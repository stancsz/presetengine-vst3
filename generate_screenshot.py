from PIL import Image, ImageDraw, ImageFont
import math

from PIL import Image, ImageDraw, ImageFont
import math

# Constants matching C++ Code
WIDTH = 900
HEIGHT = 500

COLOR_BG = "#121212"
COLOR_HEADER_BG = "#1e1e1e"
COLOR_ACCENT = "#00bcd4" # Cyan
COLOR_ACCENT_STR = "#00E5FF" # The one used in LookAndFeel
COLOR_TEXT_MAIN = "#e0e0e0"
COLOR_TEXT_DIM = "#888888"
COLOR_EDITOR_BG = "#1e1e1e"
COLOR_SLIDER_TRACK = "#2d2d2d" # Dark Grey
COLOR_SLIDER_BG = "#1e1e1e"

# Fonts
try:
    font_title = ImageFont.truetype("arialbd.ttf", 24)
    font_label = ImageFont.truetype("arialbd.ttf", 10)
    font_code = ImageFont.truetype("consola.ttf", 12)
    font_std = ImageFont.truetype("arial.ttf", 12)
except IOError:
    font_title = ImageFont.load_default()
    font_label = ImageFont.load_default()
    font_code = ImageFont.load_default()
    font_std = ImageFont.load_default()

def draw_rotary(draw, x, y, size, value, name):
    # Center
    cx = x + size // 2
    cy = y + size // 2
    radius = (size // 2) - 4

    # Angles (approx Juce default rotary)
    start_angle = 135 + 90
    end_angle = 405 + 90
    current_angle = start_angle + value * (end_angle - start_angle)

    # Convert to PIL angles (0 is right, clockwise)
    # PIL arc: start and end angles in degrees, measured clockwise from 3 o'clock.
    # JUCE 0 is 12 o'clock? No, JUCE 0 is 12 o'clock usually.
    # Let's just approximate visually.
    
    # 2. Draw Background (Dark Track) - DarkGrey
    # In C++: g.setColour (juce::Colours::darkgrey); g.drawEllipse (rx, ry, rw, rw, 2.0f);
    bbox = [cx-radius, cy-radius, cx+radius, cy+radius]
    draw.ellipse(bbox, outline="darkgrey", width=2)

    # 3. Draw Active Arc (Cyan Progress)
    # In C++: p.addCentredArc...
    # PIL arc angles are clockwise from 3 o'clock.
    # 135 degrees in JUCE (usually 0 is up) -> approx 225 in PIL?
    # Let's just use standard knob angles: 
    # Start: 135 deg (South West)
    # End: 405 deg (South East)
    pil_start = 135
    pil_end = 135 + (value * 270)
    
    draw.arc(bbox, start=pil_start, end=pil_end, fill=COLOR_ACCENT_STR, width=2)

    # 4. Draw Needle/Indicator
    # Calculate tip position
    angle_rad = math.radians(pil_end)
    # PIL coordinates: x + cos, y + sin
    tip_x = cx + math.cos(angle_rad) * radius
    tip_y = cy + math.sin(angle_rad) * radius
    
    # Draw a line from slightly inside to tip
    inner_r = radius - 10
    inner_x = cx + math.cos(angle_rad) * inner_r
    inner_y = cy + math.sin(angle_rad) * inner_r
    
    draw.line([inner_x, inner_y, tip_x, tip_y], fill="white", width=4)

    # Label
    w = draw.textlength(name, font=font_label)
    draw.text((cx - w/2, y + size), name, fill=COLOR_TEXT_DIM, font=font_label)

def draw_linear(draw, x, y, w, h, value, name):
    # Vertical Fader
    # Track
    track_w = 4
    track_x = x + (w - track_w) // 2
    draw.rectangle([track_x, y, track_x + track_w, y + h], fill=COLOR_SLIDER_TRACK)

    # Fill
    fill_h = h * value
    draw.rectangle([track_x, y + h - fill_h, track_x + track_w, y + h], fill=COLOR_ACCENT, width=0)

    # Handle
    handle_w = 16
    handle_h = 8
    handle_y = y + h - fill_h - (handle_h // 2)
    handle_x = x + (w - handle_w) // 2
    draw.rectangle([handle_x, handle_y, handle_x + handle_w, handle_y + handle_h], fill="white")

    # Label
    text_w = draw.textlength(name, font=font_label)
    draw.text((x + (w - text_w)/2, y + h + 5), name, fill=COLOR_TEXT_DIM, font=font_label)

def generate_screenshot():
    img = Image.new("RGB", (WIDTH, HEIGHT), COLOR_BG)
    draw = ImageDraw.Draw(img)

    # --- Header ---
    draw.rectangle([0, 0, WIDTH, 40], fill=COLOR_HEADER_BG)
    draw.line([0, 40, WIDTH, 40], fill="black")
    
    # Title
    title_text = "Preset Engine"
    title_font = ImageFont.truetype("arialbd.ttf", 20) if "arialbd.ttf" in str(font_title.path) else font_title
    w = draw.textlength(title_text, font=title_font)
    draw.text(((WIDTH - w) / 2, 2), title_text, fill=COLOR_ACCENT, font=title_font)
    
    # Subtitle
    sub_text = "Nap-Tech"
    try:
        font_sub = ImageFont.truetype("ariali.ttf", 12)
    except:
        font_sub = font_std
    
    w_sub = draw.textlength(sub_text, font=font_sub)
    draw.text(((WIDTH - w_sub) / 2, 24), sub_text, fill="grey", font=font_sub)

    # --- Layout Areas ---
    # Left: 40% (360px)
    # Right: 60% (540px)

    # Left Column Content
    # 1. Spectrum (Top 33% of remaining 460px = ~150px)
    spectrum_h = 150
    spectrum_y = 50 
    spectrum_w = 340 
    spectrum_x = 10

    # Draw Spectrum BG
    draw.rectangle([spectrum_x, spectrum_y, spectrum_x + spectrum_w, spectrum_y + spectrum_h], fill="#121212")
    
    # Draw FFT-like Spectrum
    # Input (Grey)
    points_in = []
    points_out = []
    for i in range(spectrum_w):
        x_norm = i / spectrum_w
        # Logarithmic-ish frequency mapping simulation
        freq = math.pow(10, x_norm * 3) # 1 to 1000
        
        # Simulate some spectral content
        val_in = (math.sin(freq * 0.1) * 0.5 + 0.5) * (1.0 - x_norm)
        val_out = val_in * (0.8 + 0.2 * math.sin(x_norm * 20)) # Processed
        
        y_in = spectrum_h - (val_in * spectrum_h * 0.8)
        y_out = spectrum_h - (val_out * spectrum_h * 0.8)
        
        points_in.append((spectrum_x + i, spectrum_y + y_in))
        points_out.append((spectrum_x + i, spectrum_y + y_out))

    # Fill Input
    points_in.append((spectrum_x + spectrum_w, spectrum_y + spectrum_h))
    points_in.append((spectrum_x, spectrum_y + spectrum_h))
    draw.polygon(points_in, fill="#444444", outline=None) # Grey

    # Fill Output
    points_out.append((spectrum_x + spectrum_w, spectrum_y + spectrum_h))
    points_out.append((spectrum_x, spectrum_y + spectrum_h))
    # PIL doesn't support alpha fill easily on RGB images without converting to RGBA
    # We'll just draw outline for output to simulate the "Cyan" look
    draw.line(points_out[:-2], fill=COLOR_ACCENT_STR, width=1)


    # 2. Controls Strip
    # languageBox (70), presetBox (120), exampleButton (60), applyButton (70)
    controls_y = spectrum_y + spectrum_h + 10
    curr_x = 10
    
    # Language
    draw.rectangle([curr_x, controls_y, curr_x + 70, controls_y + 25], fill="#1e1e1e", outline="#555")
    draw.text((curr_x + 10, controls_y+5), "YAML", fill="white", font=font_std)
    curr_x += 75
    
    # Preset
    draw.rectangle([curr_x, controls_y, curr_x + 120, controls_y + 25], fill="#1e1e1e", outline="#555")
    draw.text((curr_x + 10, controls_y+5), "Default", fill="white", font=font_std)
    # Chevron
    draw.line([curr_x + 105, controls_y + 10, curr_x + 110, controls_y + 15, curr_x + 115, controls_y + 10], fill="lightgrey", width=2)
    curr_x += 125
    
    # Reset
    draw.rectangle([curr_x, controls_y, curr_x + 60, controls_y + 25], fill="#2d2d2d", outline="#555")
    draw.text((curr_x + 10, controls_y+5), "Reset", fill="white", font=font_std)
    curr_x += 65
    
    # Apply
    draw.rectangle([curr_x, controls_y, curr_x + 70, controls_y + 25], fill="#2d2d2d", outline="#555")
    draw.text((curr_x + 15, controls_y+5), "APPLY", fill="white", font=font_std)

    # 3. Code Editor
    editor_y = controls_y + 35
    editor_h = HEIGHT - editor_y - 10
    draw.rectangle([10, editor_y, 350, editor_y + editor_h], fill=COLOR_EDITOR_BG)
    code_text = """- type: Gain
  gain_db:
    value: -6.0
    ui: Slider
    style: Rotary

- type: Filter
  mode: LowPass
  frequency: 1000.0"""
    draw.text((20, editor_y + 10), code_text, fill=COLOR_TEXT_MAIN, font=font_code)

    # --- Right Column (FlexBox Layout Simulation) ---
    right_x = 370
    right_w = WIDTH - right_x - 10
    
    # We will draw the "Default" preset components
    # Gain Block
    y = 50
    draw.rounded_rectangle([right_x, y, right_x + right_w, y + 110], radius=6, outline="#3a3a3a", fill="#2a2a2a")
    draw.text((right_x + 10, y + 5), "GAIN", fill=COLOR_ACCENT, font=font_label)
    
    # Gain Knob
    draw_rotary(draw, right_x + 20, y + 25, 70, 0.4, "GAIN DB")

    # Filter Block
    y += 115
    draw.rounded_rectangle([right_x, y, right_x + right_w, y + 110], radius=6, outline="#3a3a3a", fill="#2a2a2a")
    draw.text((right_x + 10, y + 5), "FILTER", fill=COLOR_ACCENT, font=font_label)
    
    # Filter Controls
    # Mode (ComboBox)
    draw.rectangle([right_x + 20, y + 40, right_x + 110, y + 65], fill="#1e1e1e", outline="#555")
    draw.text((right_x + 30, y + 45), "LowPass", fill="white", font=font_std)
    draw.text((right_x + 20 + 35, y + 70), "MODE", fill=COLOR_TEXT_DIM, font=font_label)

    # Freq Knob
    draw_rotary(draw, right_x + 130, y + 25, 70, 0.6, "FREQUENCY")

    img.save("screenshot.png")
    print("Screenshot generated: screenshot.png")

if __name__ == "__main__":
    generate_screenshot()
