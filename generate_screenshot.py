from PIL import Image, ImageDraw, ImageFont

# Canvas size
WIDTH, HEIGHT = 800, 600

# Colors (from ModernLookAndFeel.h)
BG_COLOR = (30, 30, 30) # 0xff1e1e1e
EDITOR_BG_COLOR = (37, 37, 38) # 0xff252526
EDITOR_TEXT_COLOR = (212, 212, 212) # 0xffd4d4d4
BUTTON_COLOR = (14, 99, 156) # 0xff0e639c
BUTTON_TEXT_COLOR = (255, 255, 255)
STATUS_TEXT_COLOR = (128, 128, 128) # Grey
PANEL_BG_COLOR = (45, 45, 48) # 0xff2d2d30
PANEL_BORDER_COLOR = (0, 0, 0, 76) # Alpha 0.3

# Fonts
try:
    font_title = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 22)
    font_code = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 15)
    font_button = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16)
    font_status = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14)
    font_label_bold = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 14)
    font_label_small = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 11)
except IOError:
    font_title = ImageFont.load_default()
    font_code = ImageFont.load_default()
    font_button = ImageFont.load_default()
    font_status = ImageFont.load_default()
    font_label_bold = ImageFont.load_default()
    font_label_small = ImageFont.load_default()

def draw_common_ui(draw, title_text, btn_text):
    # Header
    draw.text((25, 30), "Preset Engine Plugin", font=font_title, fill=(255, 255, 255))

    # Toggle Button (Top Right)
    btn_w, btn_h = 200, 35
    btn_x, btn_y = WIDTH - 25 - btn_w, 25
    draw.rounded_rectangle((btn_x, btn_y, btn_x + btn_w, btn_y + btn_h), 6, fill=BUTTON_COLOR)
    draw.text((btn_x + 20, btn_y + 8), btn_text, font=font_button, fill=BUTTON_TEXT_COLOR)

    # Footer
    draw.text((25, HEIGHT - 35), "Status: Ready.", font=font_status, fill=STATUS_TEXT_COLOR)

def generate_code_view():
    image = Image.new("RGB", (WIDTH, HEIGHT), BG_COLOR)
    draw = ImageDraw.Draw(image)

    draw_common_ui(draw, "Preset Engine Plugin", "Switch to Visual View")

    # Editor Area
    area_x, area_y = 25, 80
    area_w, area_h = WIDTH - 50, HEIGHT - 130

    draw.rectangle((area_x, area_y, area_x + area_w, area_y + area_h), fill=EDITOR_BG_COLOR)

    code_text = """# My Custom Effect Chain
- type: Gain
  gain: 1.0
- type: Compressor
  threshold: -15.0
  ratio: 4.0
  attack: 10.0
  release: 100.0
- type: Filter
  type: LowPass
  frequency: 2500.0
  q: 0.707
- type: Delay
  time: 450.0
  feedback: 0.3
  mix: 0.4
- type: Reverb
  room_size: 0.6
  wet: 0.3
"""
    text_x = area_x + 15
    text_y = area_y + 15
    line_height = 20
    for i, line in enumerate(code_text.split('\n')):
        # Simple syntax highlighting simulation
        color = EDITOR_TEXT_COLOR
        if line.strip().startswith("#"):
            color = (87, 166, 74) # Comment Green
        elif line.strip().startswith("- type:"):
            color = (86, 156, 214) # Keyword Blue
        elif ":" in line:
            color = (156, 220, 254) # Property Blue

        draw.text((text_x, text_y + i * line_height), line, font=font_code, fill=color)

    # Apply Button
    btn_w, btn_h = 180, 35
    btn_x = WIDTH - 25 - btn_w
    btn_y = HEIGHT - 45
    draw.rounded_rectangle((btn_x, btn_y, btn_x + btn_w, btn_y + btn_h), 6, fill=BUTTON_COLOR)
    draw.text((btn_x + 15, btn_y + 8), "Apply Configuration", font=font_button, fill=BUTTON_TEXT_COLOR)

    image.save("screenshot_code.png")
    print("Generated screenshot_code.png")

def draw_knob(draw, x, y, label, value_norm=0.5):
    # Simple circle knob representation
    r = 22
    cx, cy = x + 30, y + 30
    draw.ellipse((cx - r, cy - r, cx + r, cy + r), outline=(100, 100, 100), width=2, fill=(60, 60, 60))

    # Indicator line based on value (0..1) -> -135 to +135 degrees
    import math
    angle = -135 + value_norm * 270
    rad = math.radians(angle - 90) # -90 because 0 is right, we want up as reference? No, standard trig.

    # Correction for visual knob
    rad = math.radians(angle)

    ex = cx + math.sin(rad) * (r - 4)
    ey = cy - math.cos(rad) * (r - 4)

    draw.line((cx, cy, ex, ey), fill=(220, 220, 220), width=3)

    # Label
    draw.text((x + 5, y + 60), label, font=font_label_small, fill=(180, 180, 180))

def generate_visual_view():
    image = Image.new("RGB", (WIDTH, HEIGHT), BG_COLOR)
    draw = ImageDraw.Draw(image, "RGBA")

    draw_common_ui(draw, "Preset Engine Plugin", "Switch to Code View")

    area_x, area_y = 25, 80
    panel_h = 90
    spacing = 10

    effects = [
        ("Gain", ["Gain"], [0.5]),
        ("Compressor", ["Thresh", "Ratio", "Attack", "Release"], [0.7, 0.4, 0.2, 0.3]),
        ("Filter (LowPass)", ["Freq", "Q"], [0.3, 0.5]),
        ("Delay", ["Time", "Feedbk", "Mix"], [0.4, 0.3, 0.4]),
        ("Reverb", ["Size", "Damp", "Wet", "Dry", "Width"], [0.6, 0.5, 0.3, 0.7, 1.0])
    ]

    current_y = area_y

    for name, params, values in effects:
        # Panel BG
        draw.rounded_rectangle((area_x, current_y, WIDTH - 50, current_y + panel_h), 6, fill=PANEL_BG_COLOR)
        draw.rounded_rectangle((area_x, current_y, WIDTH - 50, current_y + panel_h), 6, outline=PANEL_BORDER_COLOR, width=1)

        # Name
        draw.text((area_x + 15, current_y + 35), name, font=font_label_bold, fill=(220, 220, 220))

        # Knobs
        knob_start_x = area_x + 150
        knob_spacing = 80
        for i, param in enumerate(params):
            val = values[i] if i < len(values) else 0.5
            draw_knob(draw, knob_start_x + i * knob_spacing, current_y, param, val)

        current_y += panel_h + spacing

    image.save("screenshot_visual.png")
    print("Generated screenshot_visual.png")

if __name__ == "__main__":
    generate_code_view()
    generate_visual_view()
