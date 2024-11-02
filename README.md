# The d20 maker
###### A tool for plotting the net of an icosahedron

### How-to
#### Compiling on Linux
You'll need to have `gcc`, `make`, `SDL2 dev`, `SDL2-image dev`, `SDL2-ttf dev` installed on your system.
From that point onwards, just run `make` from the main directory.
#### Compiling on Windows
I uh...
It depends on your toolchain, I guess.
#### Compiling on Mac OSX
No idea.
#### Running
First argument to the program specifies an input image:
e.g. `d20-maker my-image.png`
If no image is specified, `default.jpg` (a pretty tiger) will be used.
##### For Windows users
There's a `start.bat` provided, allowing you to use a file browser to select your input image.
Or you could just drag and drop the file you want.
#### Controlling
All you need is a mouse / touchpad device featuring scroll functionality.
A triangle is placed by using left mouse button.
##### The first triangle
Move the triangle around with your mouse. When placing your first triangle, you may use the scroll-wheel to resize or rotate the triangle. Pressing the scroll-wheel or right mouse button will let you switch between resizing and rotating.
##### All other triangles
After having placed the first triangle, a 'reset' button will be provided - allowing for clearing all triangles and starting over, without restarting the program.
Select between possible triangles by hovering over them with the mouse - you may run out of available triangles underway.
##### Adjusting and exporting the final image
When all 20 triangles have been placed, you can move then around using the left mouse button.
There will be provided a button for saving the generated image, as well as a fader for choosing folding line visibility.
The interface of these will be located to the right of the image.
#### Output
All saved images will be stored in `.bmp` format, with a unique file name. They will be placed in the folder named `output`.


### To-do list
 - [ ] Write a good README
 - [x] Basic functionality implemented
 - [ ] Write a longer to-do list
 - [x] Implement "Reset" button
 - [x] Implement "Save" button
 - [ ] Implement "Undo" feature
 - [x] Implement "Move" feature
 - [x] Implement folding line controls
 - [ ] Make it look prettier
 - [x] Remove memory leaks

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
