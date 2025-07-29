import tkinter as tk
from tkinter import Canvas
from PIL import Image, ImageTk, ImageDraw, ImageFont
import os

class DesktopCompanion:
    def __init__(self):
        self.root = tk.Tk()
        
        # Window configuration for transparency and always on top
        self.root.overrideredirect(True)  # Remove window decorations
        self.root.attributes('-topmost', True)  # Always on top
        self.root.attributes('-transparentcolor', 'white')  # Make white transparent
        self.root.configure(bg='white')
        
        # Variables for dragging
        self.offset_x = 0
        self.offset_y = 0
        
        # Create the main canvas
        self.canvas = Canvas(self.root, width=400, height=300, bg='white', highlightthickness=0)
        self.canvas.pack()
        
        # Create or load the character image
        self.create_character_image()
        
        # Bind mouse events for dragging
        self.canvas.bind('<Button-1>', self.start_drag)
        self.canvas.bind('<B1-Motion>', self.on_drag)
        
        # Position window in center of screen
        self.center_window()
        
    def create_character_image(self):
        """Create a simple character with speech bubble"""
        # Create a new image with transparent background
        img = Image.new('RGBA', (400, 300), (255, 255, 255, 0))
        draw = ImageDraw.Draw(img)
        
        # Character body (simple circle)
        character_x, character_y = 200, 200
        character_radius = 30
        
        # Draw character (blue circle)
        draw.ellipse([character_x - character_radius, character_y - character_radius,
                      character_x + character_radius, character_y + character_radius],
                     fill=(100, 150, 255, 255), outline=(50, 100, 200, 255), width=3)
        
        # Draw simple face
        # Eyes
        draw.ellipse([character_x - 15, character_y - 10, character_x - 5, character_y], 
                     fill=(0, 0, 0, 255))
        draw.ellipse([character_x + 5, character_y - 10, character_x + 15, character_y], 
                     fill=(0, 0, 0, 255))
        
        # Smile
        draw.arc([character_x - 15, character_y - 5, character_x + 15, character_y + 15],
                 start=0, end=180, fill=(0, 0, 0, 255), width=2)
        
        # Speech bubble
        bubble_x, bubble_y = 100, 80
        bubble_width, bubble_height = 180, 60
        
        # Bubble background
        draw.ellipse([bubble_x, bubble_y, bubble_x + bubble_width, bubble_y + bubble_height],
                     fill=(255, 255, 255, 255), outline=(0, 0, 0, 255), width=2)
        
        # Bubble tail pointing to character
        tail_points = [(bubble_x + 120, bubble_y + bubble_height),
                       (character_x - 10, character_y - character_radius),
                       (bubble_x + 140, bubble_y + bubble_height)]
        draw.polygon(tail_points, fill=(255, 255, 255, 255), outline=(0, 0, 0, 255))
        
        # Text in bubble
        try:
            # Try to use a better font if available
            font = ImageFont.truetype("asses/fonts/arial.ttf", 25)
        except:
            # Fallback to default font
            font = ImageFont.load_default()
        
        text = "Viens on va discuter!"
        
        # Get text bounding box for centering
        bbox = draw.textbbox((0, 0), text, font=font)
        text_width = bbox[2] - bbox[0]
        text_height = bbox[3] - bbox[1]
        
        text_x = bubble_x + (bubble_width - text_width) // 2
        text_y = bubble_y + (bubble_height - text_height) // 2
        
        draw.text((text_x, text_y), text, fill=(0, 0, 0, 255), font=font)
        
        # Save the image
        img.save('assets/textures/character.png')
        
        # Convert to PhotoImage for tkinter
        self.character_image = ImageTk.PhotoImage(img)
        
        # Add image to canvas
        self.canvas.create_image(200, 150, image=self.character_image)
        
    def center_window(self):
        """Center the window on screen"""
        self.root.update_idletasks()
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()
        
        x = (screen_width - 400) // 2
        y = (screen_height - 300) // 2
        
        self.root.geometry(f"400x300+{x}+{y}")
        
    def start_drag(self, event):
        """Start dragging the window"""
        self.offset_x = event.x
        self.offset_y = event.y
        
    def on_drag(self, event):
        """Handle window dragging"""
        x = self.root.winfo_pointerx() - self.offset_x
        y = self.root.winfo_pointery() - self.offset_y
        self.root.geometry(f"+{x}+{y}")
        
    def run(self):
        """Start the application"""
        self.root.mainloop()

if __name__ == "__main__":
    app = DesktopCompanion()
    app.run()
