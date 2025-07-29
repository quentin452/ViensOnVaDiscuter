# ViensOnVaDiscuter

A simple desktop companion application with a draggable character that says "Viens on va discuter!" in a speech bubble.

## Features
- Transparent window background
- Draggable character with mouse
- Always stays on top of other windows
- Speech bubble with custom text
- Auto-generated character image

## Requirements
- Python 3.7+
- Pillow (PIL) library for image handling

## Installation & Usage

1. Install required packages:
```bash
pip install -r requirements.txt
```

2. Run the application:
```bash
python main.py
```

Or simply double-click `run.bat` on Windows.

## How it works
- The window has a transparent background (white areas become invisible)
- Click and drag anywhere on the character to move it around your desktop
- The character and speech bubble are drawn programmatically
- The application stays on top of all other windows

## Customization
You can easily modify the character appearance, text, colors, and size by editing the `create_character_image()` method in `main.py`.