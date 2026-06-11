// =============================================================================
//  pwm_motor_ctrl.v — FPGA Motor Controller for Shrike (SLG47910)
//  See README.md for full SPI protocol and pin mapping details.
// =============================================================================

(* top *)
module pwm_motor_ctrl (
    (* iopad_external_pin, clkbuf_inhibit *) input  wire clk,            // FPGA internal oscillator (~25 MHz)
    (* iopad_external_pin *) input  wire rst_n,          // Active-low reset (active-low)

    // SPI slave interface (directly from MCU)
    (* iopad_external_pin *) input  wire spi_cs_n,       // Chip select, active-low
    (* iopad_external_pin *) input  wire spi_sclk,       // SPI clock from MCU
    (* iopad_external_pin *) input  wire spi_mosi,       // SPI data in from MCU

    // Motor driver outputs (directly to L298N)
    (* iopad_external_pin *) output wire pwm_out,        // → L298N ENA
    (* iopad_external_pin *) output wire motor_in1,      // → L298N IN1
    (* iopad_external_pin *) output wire motor_in2       // → L298N IN2
);

    // ─── Parameters ──────────────────────────────────────────────────────
    parameter PRESCALER_BITS = 2;   // prescaler = 2^N, divides clk by 4

    // ─── SPI Slave ───────────────────────────────────────────────────────
    //  Shift in 16 bits (2 bytes), latch on CS rising edge.

    reg [15:0] spi_shift;
    reg [4:0]  spi_bit_cnt;
    reg        spi_sclk_prev;

    // Latched command registers
    reg [7:0]  duty;
    reg [1:0]  direction;

    // Synchronize SPI signals to FPGA clock domain
    reg spi_cs_n_sync1,  spi_cs_n_sync2;
    reg spi_sclk_sync1,  spi_sclk_sync2;
    reg spi_mosi_sync1,  spi_mosi_sync2;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            spi_cs_n_sync1 <= 1'b1;
            spi_cs_n_sync2 <= 1'b1;
            spi_sclk_sync1 <= 1'b0;
            spi_sclk_sync2 <= 1'b0;
            spi_mosi_sync1 <= 1'b0;
            spi_mosi_sync2 <= 1'b0;
        end else begin
            spi_cs_n_sync1 <= spi_cs_n;
            spi_cs_n_sync2 <= spi_cs_n_sync1;
            spi_sclk_sync1 <= spi_sclk;
            spi_sclk_sync2 <= spi_sclk_sync1;
            spi_mosi_sync1 <= spi_mosi;
            spi_mosi_sync2 <= spi_mosi_sync1;
        end
    end

    wire spi_sclk_rising = spi_sclk_sync2 & ~spi_sclk_prev;
    wire spi_cs_rising   = spi_cs_n_sync2 & ~spi_cs_n_prev;

    reg spi_cs_n_prev;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            spi_sclk_prev <= 1'b0;
            spi_cs_n_prev <= 1'b1;
            spi_shift     <= 16'd0;
            spi_bit_cnt   <= 5'd0;
            duty          <= 8'd0;
            direction     <= 2'b00;
        end else begin
            spi_sclk_prev <= spi_sclk_sync2;
            spi_cs_n_prev <= spi_cs_n_sync2;

            if (!spi_cs_n_sync2) begin
                // CS is active — shift in data on SCLK rising edges
                if (spi_sclk_rising) begin
                    spi_shift   <= {spi_shift[14:0], spi_mosi_sync2};
                    spi_bit_cnt <= spi_bit_cnt + 1;
                end
            end else begin
                // CS just went high — latch the received data
                if (spi_cs_rising && spi_bit_cnt >= 5'd16) begin
                    direction <= spi_shift[9:8];
                    duty      <= spi_shift[7:0];
                end
                spi_bit_cnt <= 5'd0;
            end
        end
    end

    // ─── PWM Generator ──────────────────────────────────────────────────
    //  Free-running counter with prescaler for PWM frequency control.

    reg [PRESCALER_BITS + 7 : 0] pwm_counter;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            pwm_counter <= 0;
        else
            pwm_counter <= pwm_counter + 1;
    end

    // Top 8 bits of the counter compared against duty cycle
    wire [7:0] pwm_phase = pwm_counter[PRESCALER_BITS + 7 : PRESCALER_BITS];

    // PWM output: HIGH when counter < duty, respecting direction (0 = stopped)
    wire motor_active = (direction != 2'b00);
    assign pwm_out = motor_active & (pwm_phase < duty);

    // ─── Direction Decoder ──────────────────────────────────────────────
    //  01 = forward:  IN1=H, IN2=L
    //  10 = reverse:  IN1=L, IN2=H
    //  00 = stop:     IN1=L, IN2=L  (coast)
    //  11 = brake:    IN1=H, IN2=H  (short brake)

    assign motor_in1 = direction[0];
    assign motor_in2 = direction[1];

endmodule
