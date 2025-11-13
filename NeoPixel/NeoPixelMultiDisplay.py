import board
import neopixel
from PIL import Image
import time

# ======================================================
# === CONFIGURATION ===
# ======================================================

# Panel configuration
PANEL_WIDTH = 16
PANEL_HEIGHT = 16
PANELS_X = 2
PANELS_Y = 2

# Wiring mode
SERPENTINE_PIXELS = True      # Serpentine *within each panel*
SERPENTINE_DISPLAYS = True    # Serpentine *across panel rows*

# NeoPixel config
DATA_PIN = board.D21
BRIGHTNESS = 0.05

# Image behavior
SCALE_TO_FIT = True    # Toggle image scaling (True = resize to fit display)
IMAGE_SOURCE = None    # Example: "smiley.png" or None for manual list

# ======================================================
# === AUTO-CALCULATED VALUES ===
# ======================================================

DISPLAY_WIDTH = PANEL_WIDTH * PANELS_X
DISPLAY_HEIGHT = PANEL_HEIGHT * PANELS_Y
NUM_PIXELS = DISPLAY_WIDTH * DISPLAY_HEIGHT

pixels = neopixel.NeoPixel(DATA_PIN, NUM_PIXELS, auto_write=False)
pixels.brightness = BRIGHTNESS

# ======================================================
# === PIXEL MAPPING FUNCTION ===
# ======================================================

def build_pixel_map():
    """Build pixel map with optional serpentine wiring for panels and pixels."""
    index_map = []
    panel_size = PANEL_WIDTH * PANEL_HEIGHT

    for panel_y in range(PANELS_Y):
        # Reverse panel order every other row if serpentine across panels
        panel_row_reverse = SERPENTINE_DISPLAYS and (panel_y % 2 == 1)

        for row in range(PANEL_HEIGHT):
            # Reverse every other pixel row if serpentine inside panel
            pixel_row_reverse = SERPENTINE_PIXELS and (row % 2 == 1)

            for panel_x in range(PANELS_X):
                # Choose actual panel order depending on row direction
                actual_panel_x = panel_x
                if panel_row_reverse:
                    actual_panel_x = PANELS_X - 1 - panel_x

                base_index = (panel_y * PANELS_X + actual_panel_x) * panel_size

                for col in range(PANEL_WIDTH):
                    x = col
                    if pixel_row_reverse:
                        x = PANEL_WIDTH - 1 - x

                    local_index = row * PANEL_WIDTH + x
                    index_map.append(base_index + local_index)

    return index_map


# ======================================================
# === IMAGE LOADING & PROCESSING (POSITION AWARE) ===
# ======================================================

def load_image_data(source, width, height, scale_to_fit=True, pos_x=0, pos_y=0):
    if isinstance(source, str):
        img = Image.open(source).convert("RGB")
        img_w, img_h = img.size

        if scale_to_fit:
            img.thumbnail((width, height), Image.LANCZOS)
            img_w, img_h = img.size

        canvas = Image.new("RGB", (width, height), (0, 0, 0))
        pos_x = max(min(pos_x, width - img_w), 0)
        pos_y = max(min(pos_y, height - img_h), 0)
        canvas.paste(img, (pos_x, pos_y))
        return list(canvas.getdata())

    elif isinstance(source, list):
        if any(isinstance(row, list) for row in source):
            flat_source = [px for row in source for px in row]
            logical_height = len(source)
            logical_width = len(source[0])
        else:
            flat_source = source
            total_pixels = len(source)
            logical_width = int(total_pixels ** 0.5)
            logical_height = total_pixels // logical_width

        canvas = [(0, 0, 0)] * (width * height)
        for y in range(logical_height):
            for x in range(logical_width):
                src_idx = y * logical_width + x
                dst_x = x + pos_x
                dst_y = y + pos_y
                if 0 <= dst_x < width and 0 <= dst_y < height:
                    canvas[dst_y * width + dst_x] = flat_source[src_idx]
        return canvas

    else:
        raise ValueError("Unsupported image source type. Must be file path or RGB list.")


# ======================================================
# === DISPLAY FUNCTION ===
# ======================================================

def show_image(image_source, pos_x=0, pos_y=0):
    mapping = build_pixel_map()
    image_data = load_image_data(image_source, DISPLAY_WIDTH, DISPLAY_HEIGHT, SCALE_TO_FIT, pos_x, pos_y)

    if len(image_data) < NUM_PIXELS:
        image_data += [(0, 0, 0)] * (NUM_PIXELS - len(image_data))
    elif len(image_data) > NUM_PIXELS:
        image_data = image_data[:NUM_PIXELS]

    for logical_index, pixel_index in enumerate(mapping):
        if logical_index < len(image_data):
            pixels[pixel_index] = image_data[logical_index]

    pixels.show()
    print(f"✅ Display updated at position ({pos_x}, {pos_y})")


# ======================================================
# === TEST EXAMPLES ===
# ======================================================

if __name__ == "__main__":
    if IMAGE_SOURCE:
        show_image(IMAGE_SOURCE, pos_x=4, pos_y=4)
    else:
        heart_8x8 = [
            [(0,0,0), (255,0,0), (255,0,0), (0,0,0), (0,0,0), (255,0,0), (255,0,0), (0,0,0)],
            [(255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0)],
            [(255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0)],
            [(0,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (0,0,0)],
            [(0,0,0), (0,255,0), (255,0,0), (255,0,0), (255,0,0), (255,0,0), (0,255,0), (0,0,0)],
            [(0,0,0), (0,0,255), (0,255,0), (255,0,0), (255,0,0), (0,255,0), (0,0,255), (0,0,0)],
            [(0,0,0), (0,0,0), (0,255,0), (255,0,0), (0,255,0), (0,0,0), (0,0,0), (0,0,0)],
            [(0,0,0), (0,0,0), (0,0,255), (0,255,0), (0,0,255), (0,0,0), (0,0,0), (0,0,0)],
        ]

        show_image(heart_8x8, pos_x=8, pos_y=8)

        while True:
            for x in range(0, DISPLAY_WIDTH - 8):
                show_image(heart_8x8, pos_x=x, pos_y=12)
                time.sleep(0.05)
