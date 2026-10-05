# PNG Image Processor

A command-line utility written in C for processing and manipulating PNG images. This tool provides various operations, including drawing shapes, applying ornamental frames, and rotating specific regions of an image.

## Features & Usage

### 1. Draw a Rectangle
Use the `-rect` flag to draw a rectangle on the image. 

**Flags:**
* `-left_up <x.y>`: Coordinates of the top-left corner (e.g., `10.15`).
* `-right_down <x.y>`: Coordinates of the bottom-right corner.
* `-thickness <number>`: Line thickness (must be greater than 0).
* `-color <r.g.b>`: Line color specified as RGB values (e.g., `-color 255.0.0` for red).
* `-fill`: Optional flag. If provided, the rectangle will be filled.
* `-fill_color <r.g.b>`: The fill color (works exactly like the `-color` flag). Applicable only if `-fill` is present.

### 2. Ornamental Frame
Use the `-ornament` flag to generate a patterned frame around the image.

**Flags:**
* `-pattern <type>`: The pattern type. Required values include `rectangle`, `circle`, or `semicircles`. 
* `-color <r.g.b>`: Color of the pattern, specified as RGB values (e.g., `255.0.0`).
* `-thickness <number>`: Width of the frame (must be greater than 0).
* `-count <number>`: Quantity of pattern elements (must be greater than 0).

### 3. Rotate Image Region
Use the `-rotate` flag to rotate a specific rectangular region (or the entire image) by a fixed angle.

**Flags:**
* `-left_up <x.y>`: Coordinates of the top-left corner of the target area.
* `-right_down <x.y>`: Coordinates of the bottom-right corner of the target area.
* `-angle <degrees>`: The rotation angle. Supported values are `90`, `180`, and `270`.
