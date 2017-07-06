# The d20 Maker
###### A tool for creating the net of an icosahedron

### How-to
#### Compiling on Linux
You'll need to have `gcc`, `SDL2`, `SDL2_image`, `SDL2_ttf` installed on your system.
From that point onwards, just run `make` from the main directory.
#### Running
First argument to the program specifies an input image:
e.g. `d20-maker *my-image.png`*
If no image is specified, `default.jpg` will be used.
##### For Windows users
There's a `start.bat` provided, allowing you to use a file browser to select your input image.
#### Controlling
All you need is a mouse with a scroll-wheel.
Select between possible triangles by hovering over them with the mouse.
A selected triangle is placed by using left mouse button.
When placing your first triangle, you may use the scroll-wheel to resize or rotate the triangle. Pressing the scroll-wheel or right mouse button will let you switch between resizing and rotating.


### To-do list
 - [ ] Write a good README
 - [x] Basic functionality implemented
 - [ ] Write a longer to-do list
 - [ ] Implement "Reset" button
 - [ ] Implement "Save" button
 - [ ] Implement "Undo" feature
 - [ ] Implement "Move" feature

### Supported file extensions
 + `.bmp`, `.dib`
 + `.gif`
 + `.jpg`, `.jpeg`, `.jpe`, `.jif`, `.jfif`, `.jfi`
 + `.iff`, `.lbm`
 + `.pcx`
 + `.png`
 + `.pbm`, `.pgm`, `.ppm`, `.pnm`
 + `.tga`, `.icb`, `.vda`, `.vst`
 + `.tif`, `.tiff`
 + `.webp`
 + `.xcf`
 + `.xpm`
 + `.xv`
