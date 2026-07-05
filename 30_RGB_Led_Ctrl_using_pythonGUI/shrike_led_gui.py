# gui to control the rgb led over serial
# pip install customtkinter pyserial CTkColorPicker

import customtkinter as ctk
from CTkColorPicker import AskColor
import serial
import serial.tools.list_ports
import threading
import queue
import colorsys
import time

ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

BAUD = 115200
SEND_THROTTLE_S = 0.02   # cap how fast slider drags can spam serial


class ShrikeLEDApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title("Shrike RGB LED Control")
        self.geometry("420x520")
        self.ser = None
        self.rgb = [0, 0, 0]
        self.effect_mode = None  # None, "breath", or "disco"
        self.last_send_time = 0

        # connect + read run on background threads so the ui never freezes
        self.stop_read = threading.Event()
        self.read_thread = None
        self.msg_queue = queue.Queue()

        # port picker + connect button
        port_frame = ctk.CTkFrame(self)
        port_frame.pack(pady=10, padx=10, fill="x")

        self.port_menu = ctk.CTkOptionMenu(port_frame, values=self.list_ports())
        self.port_menu.pack(side="left", padx=5, expand=True, fill="x")

        ctk.CTkButton(port_frame, text="Refresh", width=70,
                      command=self.refresh_ports).pack(side="left", padx=5)

        self.connect_btn = ctk.CTkButton(port_frame, text="Connect",
                                          width=90, command=self.toggle_connect)
        self.connect_btn.pack(side="left", padx=5)

        self.status_label = ctk.CTkLabel(self, text="Disconnected", text_color="tomato")
        self.status_label.pack(pady=(0, 10))

        # rgb sliders
        self.sliders = {}
        self.value_labels = {}
        for ch, color in (("R", "#ff5555"), ("G", "#55ff55"), ("B", "#5599ff")):
            row = ctk.CTkFrame(self)
            row.pack(pady=6, padx=15, fill="x")

            ctk.CTkLabel(row, text=ch, text_color=color, width=20).pack(side="left")

            s = ctk.CTkSlider(row, from_=0, to=255, number_of_steps=255,
                               command=lambda v, c=ch: self.on_slide(c, v))
            s.set(0)
            s.pack(side="left", expand=True, fill="x", padx=8)
            self.sliders[ch] = s

            lbl = ctk.CTkLabel(row, text="0", width=30)
            lbl.pack(side="left")
            self.value_labels[ch] = lbl

        # hue slider - live updates while dragging, same deal as the rgb sliders
        hue_row = ctk.CTkFrame(self)
        hue_row.pack(pady=(10, 6), padx=15, fill="x")
        ctk.CTkLabel(hue_row, text="Hue", width=30).pack(side="left")
        self.hue_slider = ctk.CTkSlider(hue_row, from_=0, to=360, number_of_steps=360,
                                         command=self.on_hue)
        self.hue_slider.set(0)
        self.hue_slider.pack(side="left", expand=True, fill="x", padx=8)

        btn_frame = ctk.CTkFrame(self)
        btn_frame.pack(pady=15, padx=15, fill="x")

        ctk.CTkButton(btn_frame, text="Pick Colour",
                      command=self.pick_color).pack(side="left", expand=True,
                                                     fill="x", padx=5)
        ctk.CTkButton(btn_frame, text="All Off", fg_color="firebrick",
                      hover_color="darkred",
                      command=self.all_off).pack(side="left", expand=True,
                                                  fill="x", padx=5)

        # breath and disco just tell the board to animate on its own
        effect_frame = ctk.CTkFrame(self)
        effect_frame.pack(pady=(0, 5), padx=15, fill="x")

        self._idle_color = ctk.CTkButton(effect_frame, text="").cget("fg_color")

        self.breath_btn = ctk.CTkButton(effect_frame, text="🌬 Breath",
                                         command=lambda: self.toggle_effect("breath"))
        self.breath_btn.pack(side="left", expand=True, fill="x", padx=5)

        self.disco_btn = ctk.CTkButton(effect_frame, text="🎉 Disco",
                                        command=lambda: self.toggle_effect("disco"))
        self.disco_btn.pack(side="left", expand=True, fill="x", padx=5)

        self.preview = ctk.CTkFrame(self, height=60, fg_color="#000000")
        self.preview.pack(pady=10, padx=15, fill="x")

        self.after(50, self.poll_queue)  # drains the connect/read thread queue

    def list_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        return ports if ports else ["No ports found"]

    def refresh_ports(self):
        self.port_menu.configure(values=self.list_ports())

    # ---------- connection handling ----------

    def toggle_connect(self):
        if self.ser and self.ser.is_open:
            self.disconnect()
            return

        port = self.port_menu.get()
        if port == "No ports found":
            return

        self.connect_btn.configure(state="disabled", text="Connecting...")
        self.status_label.configure(text="Connecting...", text_color="orange")
        threading.Thread(target=self._connect_worker, args=(port,), daemon=True).start()

    def _connect_worker(self, port):
        ser = None
        # dtr/rts=False before open stops the board resetting, retry a few
        # times in case the port's still coming back up from a prior reset
        for attempt in range(5):
            try:
                ser = serial.Serial()
                ser.port = port
                ser.baudrate = BAUD
                ser.timeout = 0.5
                ser.dsrdtr = False
                ser.rtscts = False
                ser.dtr = False
                ser.rts = False
                ser.open()
                break
            except (serial.SerialException, OSError):
                ser = None
                time.sleep(0.5)

        if ser is None:
            self.msg_queue.put(("conn_failed", f"couldn't open {port}"))
            return

        # short grace period to catch a READY if a reset did happen -
        # not seeing one just means the board's still running, that's fine
        ser.reset_input_buffer()
        deadline = time.time() + 1.5
        got_ready = False
        while time.time() < deadline:
            try:
                line = ser.readline().decode(errors="ignore").strip()
            except (serial.SerialException, OSError):
                self.msg_queue.put(("conn_failed", "port dropped while booting"))
                try:
                    ser.close()
                except Exception:
                    pass
                return
            if line.startswith("READY"):
                got_ready = True
                break

        self.ser = ser
        self.stop_read.clear()
        self.read_thread = threading.Thread(target=self._read_worker, daemon=True)
        self.read_thread.start()

        label = f"{port} (board reset)" if got_ready else port
        self.msg_queue.put(("connected", label))

    def _read_worker(self):
        # reads whatever the board prints so errors actually show up
        while not self.stop_read.is_set():
            try:
                if not (self.ser and self.ser.is_open):
                    return
                line = self.ser.readline().decode(errors="ignore").strip()
                if line:
                    self.msg_queue.put(("board", line))
            except (serial.SerialException, OSError):
                self.msg_queue.put(("conn_lost", "lost connection to board"))
                return

    def poll_queue(self):
        try:
            while True:
                kind, data = self.msg_queue.get_nowait()
                if kind == "connected":
                    self.status_label.configure(text=f"Connected: {data}", text_color="lightgreen")
                    self.connect_btn.configure(text="Disconnect", state="normal")
                elif kind == "conn_failed":
                    self.status_label.configure(text=f"Failed: {data}", text_color="tomato")
                    self.connect_btn.configure(text="Connect", state="normal")
                    self.ser = None
                elif kind == "conn_lost":
                    self._teardown_connection()
                    self.status_label.configure(text=data, text_color="tomato")
                    self.connect_btn.configure(text="Connect", state="normal")
                elif kind == "board":
                    if data.startswith("ERR"):
                        self.status_label.configure(text=data, text_color="orange")
        except queue.Empty:
            pass
        self.after(50, self.poll_queue)

    def _teardown_connection(self):
        self.stop_read.set()
        if self.ser:
            try:
                self.ser.close()
            except Exception:
                pass
        self.ser = None

    def disconnect(self):
        self._teardown_connection()
        self.status_label.configure(text="Disconnected", text_color="tomato")
        self.connect_btn.configure(text="Connect", state="normal")

    # ---------- sending commands ----------

    def send(self, cmd, throttle=False):
        if not (self.ser and self.ser.is_open):
            return
        if throttle:
            now = time.monotonic()
            if now - self.last_send_time < SEND_THROTTLE_S:
                return
            self.last_send_time = now
        try:
            self.ser.write((cmd + "\n").encode())
        except (serial.SerialException, OSError) as e:
            self.msg_queue.put(("conn_lost", f"write failed: {e}"))

    # ---------- ui callbacks ----------

    def _apply_rgb(self, r, g, b, update_hue=True):
        self.rgb = [r, g, b]
        for ch, v in zip(("R", "G", "B"), (r, g, b)):
            self.sliders[ch].set(v)
            self.value_labels[ch].configure(text=str(v))
        if update_hue:
            h, _, _ = colorsys.rgb_to_hsv(r / 255, g / 255, b / 255)
            self.hue_slider.set(h * 360)
        self.update_preview()
        self.send(f"RGB:{r},{g},{b}", throttle=True)

    def on_slide(self, channel, value):
        self.clear_effect()
        value = int(value)
        self.value_labels[channel].configure(text=str(value))
        idx = {"R": 0, "G": 1, "B": 2}[channel]
        self.rgb[idx] = value
        self.update_preview()
        self.send(f"{channel}:{value}", throttle=True)

    def on_hue(self, value):
        self.clear_effect()
        hue = float(value) / 360
        r, g, b = colorsys.hsv_to_rgb(hue, 1.0, 1.0)
        self._apply_rgb(int(r * 255), int(g * 255), int(b * 255), update_hue=False)

    def pick_color(self):
        pick = AskColor(title="Pick LED colour").get()
        if pick is None:
            return
        self.clear_effect()
        r, g, b = self.hex_to_rgb(pick)
        self._apply_rgb(r, g, b, update_hue=True)

    def all_off(self):
        self.clear_effect()
        self.rgb = [0, 0, 0]
        for ch in ("R", "G", "B"):
            self.sliders[ch].set(0)
            self.value_labels[ch].configure(text="0")
        self.hue_slider.set(0)
        self.update_preview()
        self.send("OFF")

    def toggle_effect(self, name):
        # click again to turn it back off
        if self.effect_mode == name:
            self.effect_mode = None
            self.send("MODE:MANUAL")
        else:
            self.effect_mode = name
            self.send(f"MODE:{name.upper()}")
        self.update_effect_buttons()

    def clear_effect(self):
        if self.effect_mode is not None:
            self.effect_mode = None
            self.update_effect_buttons()

    def update_effect_buttons(self):
        self.breath_btn.configure(
            fg_color="#2fa572" if self.effect_mode == "breath" else self._idle_color)
        self.disco_btn.configure(
            fg_color="#2fa572" if self.effect_mode == "disco" else self._idle_color)

    def update_preview(self):
        r, g, b = self.rgb
        self.preview.configure(fg_color=f"#{r:02x}{g:02x}{b:02x}")

    @staticmethod
    def hex_to_rgb(hex_code):
        hex_code = hex_code.lstrip("#")
        return tuple(int(hex_code[i:i + 2], 16) for i in (0, 2, 4))


if __name__ == "__main__":
    app = ShrikeLEDApp()
    app.mainloop()