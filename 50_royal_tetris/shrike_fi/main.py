"""
Project 50: Royal Tetris
Board: Shrike Fi (ESP32-S3)
Display: SSD1306 OLED via Hardware SPI
Input: Rotary Encoder (CLK, DT, SW)
"""

from machine import Pin, SPI
from ssd1306 import SSD1306_SPI
import time
import random

# --- HARDWARE SETUP ---
spi = SPI(1, baudrate=10_000_000, sck=Pin(5), mosi=Pin(6), miso=Pin(1))
oled_cs = Pin(7, Pin.OUT)
oled_dc = Pin(4, Pin.OUT)
oled_res = Pin(3, Pin.OUT)
oled = SSD1306_SPI(128, 64, spi, oled_dc, oled_res, oled_cs)

clk = Pin(15, Pin.IN, Pin.PULL_UP)
dt  = Pin(16, Pin.IN, Pin.PULL_UP)
sw  = Pin(17, Pin.IN, Pin.PULL_UP)

last_clk = clk.value()
last_sw  = sw.value()

# --- TETRIS ENGINE ---
SHAPES = [
    [['....','####','....','....'], ['..#.','..#.','..#.','..#.']], # I
    [['...','.OO','.OO','...']], # O
    [['..#','###','...'], ['.#.','..#','..#'], ['...','###','#..'], ['#..','#..','.#.']], # L
    [['#..','###','...'], ['..#','..#','.#.'], ['...','###','..#'], ['.#.','#..','#..']], # J
    [['.#.','###','...'], ['.#.','..#','.#.'], ['...','###','.#.'], ['.#.','#..','.#.']], # T
    [['.##','##.','...'], ['.#.','.##','..#']], # S
    [['##.','.##','...'], ['..#','.##','.#.']]  # Z
]

board = []
score = 0
level = 1
lines_cleared = 0

current_piece = None
next_piece_id = random.randint(0, 6)
fall_speed = 500 # ms per fall
last_fall_time = 0

def get_coords(p_id, rot):
    shape = SHAPES[p_id][rot % len(SHAPES[p_id])]
    coords = []
    for y, row in enumerate(shape):
        for x, char in enumerate(row):
            if char != '.':
                coords.append((x, y))
    return coords

def spawn_piece():
    global current_piece, next_piece_id
    current_piece = {"id": next_piece_id, "rot": 0, "x": 3, "y": -2}
    next_piece_id = random.randint(0, 6)
    if check_collision(current_piece, 0, 0):
        return False # Game Over
    return True

def check_collision(piece, dx, dy, drot=0):
    coords = get_coords(piece["id"], piece["rot"] + drot)
    for px, py in coords:
        nx, ny = piece["x"] + px + dx, piece["y"] + py + dy
        if nx < 0 or nx >= 10 or ny >= 20:
            return True
        if ny >= 0 and board[ny][nx]:
            return True
    return False

def lock_piece():
    global score, lines_cleared, level, fall_speed
    coords = get_coords(current_piece["id"], current_piece["rot"])
    for px, py in coords:
        nx, ny = current_piece["x"] + px, current_piece["y"] + py
        if ny >= 0:
            board[ny][nx] = 1
    
    # Check for full lines
    full_lines = []
    for r in range(20):
        if all(board[r]):
            full_lines.append(r)
    
    if full_lines:
        for r in full_lines:
            del board[r]
            board.insert(0, [0]*10)
        
        # Scoring
        lines = len(full_lines)
        if lines == 1: score += 100 * level
        elif lines == 2: score += 300 * level
        elif lines == 3: score += 500 * level
        elif lines == 4: score += 800 * level
        
        lines_cleared += lines
        level = (lines_cleared // 10) + 1
        fall_speed = max(100, 500 - (level - 1) * 50)

def draw_block(x, y, filled=True):
    # 3x3 blocks, top-left of board is at (10, 2)
    bx = 10 + x * 3
    by = 2 + y * 3
    oled.fill_rect(bx, by, 3, 3, 1 if filled else 0)

def draw_game():
    oled.fill(0)
    # Draw Board Border
    oled.rect(9, 1, 32, 62, 1)
    
    # Draw locked blocks
    for y in range(20):
        for x in range(10):
            if board[y][x]:
                draw_block(x, y, True)
                
    # Draw current piece
    if current_piece:
        coords = get_coords(current_piece["id"], current_piece["rot"])
        for px, py in coords:
            ny = current_piece["y"] + py
            if ny >= 0:
                draw_block(current_piece["x"] + px, ny, True)
    
    # UI
    oled.text("TETRIS", 50, 2)
    oled.text(f"Lv:{level}", 50, 16)
    oled.text(f"Sc:{score}", 50, 28)
    oled.text("Next:", 50, 42)
    
    # Draw next piece preview
    n_coords = get_coords(next_piece_id, 0)
    for px, py in n_coords:
        # Preview drawn at ~ x=90, y=42
        oled.fill_rect(90 + px*3, 42 + py*3, 3, 3, 1)
        
    oled.show()

# --- MAIN LOOP ---
def run_game():
    global board, score, level, lines_cleared, last_fall_time, current_piece, last_clk, last_sw
    board = [[0]*10 for _ in range(20)]
    score = 0
    level = 1
    lines_cleared = 0
    
    spawn_piece()
    last_fall_time = time.ticks_ms()
    draw_game()
    
    game_over = False
    
    while not game_over:
        # Handle Encoder
        new_clk = clk.value()
        if last_clk == 1 and new_clk == 0:
            if dt.value() == 1:
                # Turn Right -> Move Right
                if not check_collision(current_piece, 1, 0):
                    current_piece["x"] += 1
                    draw_game()
            else:
                # Turn Left -> Move Left
                if not check_collision(current_piece, -1, 0):
                    current_piece["x"] -= 1
                    draw_game()
        last_clk = new_clk
        
        # Handle Button Press (Rotate)
        new_sw = sw.value()
        if last_sw == 1 and new_sw == 0:
            time.sleep_ms(20) # debounce
            if sw.value() == 0:
                # Rotate
                if not check_collision(current_piece, 0, 0, 1):
                    current_piece["rot"] += 1
                    draw_game()
        last_sw = new_sw
        
        # Gravity / Falling
        if time.ticks_diff(time.ticks_ms(), last_fall_time) > fall_speed:
            if not check_collision(current_piece, 0, 1):
                current_piece["y"] += 1
            else:
                lock_piece()
                if not spawn_piece():
                    game_over = True
            last_fall_time = time.ticks_ms()
            draw_game()
            
        time.sleep_ms(1)
        
    # Game Over screen
    oled.fill_rect(10, 20, 108, 25, 0)
    oled.rect(10, 20, 108, 25, 1)
    oled.text("GAME OVER!", 25, 25)
    oled.text(f"Score: {score}", 25, 35)
    oled.show()
    
    time.sleep(1)
    # Wait for click to return
    while sw.value() == 1:
        time.sleep_ms(10)
    while sw.value() == 0:
        time.sleep_ms(10)

while True:
    oled.fill(0)
    oled.text("ROYAL TETRIS", 16, 20)
    oled.text("Press to Start", 8, 40)
    oled.show()
    
    if sw.value() == 0:
        time.sleep_ms(50)
        while sw.value() == 0:
            pass
        run_game()
    time.sleep_ms(10)
