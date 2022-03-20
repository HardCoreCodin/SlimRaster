<img src="SlimRaster_logo.png" alt="SlimRaster_logo"><br>

<img src="src/examples/SlimRaster.gif" alt="SlimRaster" height="360"><br>

A minimalist, platform agnostic interactive real time rasterizer.<br>
Strong emphasis on simplicity, ease of use and almost no setup to get started with<br>
Written in plain C and can be complied in either C or C++<br>

This project extends [SlimEngine](https://github.com/HardCoreCodin/SlimEngine).

Features:
-
All features of <b>SlimEngine</b> are available here as well.<br>
Additional features include rasterization facilities:<br>
- Pixel Shaders that feed interpolated vertex attributes
- Mesh shaders for object culling using axis-aligned bounding boxes
- Frustum and back face triangle culling
- Frustum triangle clipping (interpolates vertex attributes)
- Perspective corrected barycentric coordinates
- Tangent space derivatives for adaptive texture mip-level selection
- Bi-linear filtered texture sampling with auto-selected mip levels
- Anti aliasing (optional SSAA)

* <b><u>obj2mesh</b>:</u> Also privided is a separate CLI tool for converting `.obj` files to `.mesh` files.<br>
  It is also written in plain C (so is compatible with C++)<br>
  Usage: `./obj2mesh src.obj trg.mesh`<br>
  - invert_winding_order : Reverses the vertex ordering (for objs exported with clockwise order)<br>

* <b><u>bmp2texture</b>:</u> Also provided is a separate CLI tool for converting `.bmp` files to `.texture` files.<br>
  It is also written in plain C (so is compatible with C++)<br>
  Usage: `./bmp2texture src.bmp trg.texture`<br>
  - m : Generate mip-maps<br>
  - w : Wrap-around<br>
  - f : Filter<br>

Architecture:
-
The platform layer only uses operating-system headers (no standard library used).<br>
The application layer itself has no dependencies, apart from the standard math header.<br>
It is just a library that the platform layer uses - it has no knowledge of the platform.<br>

More details on this architecture [here](https://youtu.be/Ev_TeQmus68).


Usage:
-
The single header file variant includes everything.<br>
Otherwise, specific headers can be included from the directory of headers.<br>
The main entry point for the app needs to be defined explicitly (see [SlimApp](https://github.com/HardCoreCodin/SlimApp)). <br>

SlimRaster comes with pre-configured CMake targets for all examples.<br>
For manual builds on Windows, the typical system libraries need to be linked<br>
(winmm.lib, gdi32.lib, shell32.lib, user32.lib) and the SUBSYSTEM needs to be set to WINDOWS<br>

SlimRaster does not come with any GUI functionality at this point.<br>
Some example apps have an optional HUD (heads up display) that shows additional information.<br>
It can be toggled on or off using the`tab` key.<br>

All examples are interactive using <b>SlimRaster</b>'s facilities having 2 interaction modes:
1. FPS navigation (WASD + mouse look + zooming)<br>
2. DCC application (default)<br>

Double clicking the `left mouse button` anywhere within the window toggles between these 2 modes.<btr>

Entering FPS mode captures the mouse movement for the window and hides the cursor.<br>
Navigation is then as in a typical first-person game (plus lateral movement and zooming):<br>

Move the `mouse` to freely look around (even if the cursor would leave the window border)<br>
Scroll the `mouse wheel` to zoom in and out (changes the field of view of the perspective)<br>
Hold `W` to move forward<br>
Hold `S` to move backward<br>
Hold `A` to move left<br>
Hold `D` to move right<br>
Hold `R` to move up<br>
Hold `F` to move down<br>

Exit this mode by double clicking the `left mouse button`.

The default interaction mode is similar to a typical DCC application (i.e: Maya):<br>
The mouse is not captured to the window and the cursor is visible.<br>
Holding the `right mouse button` and dragging the mouse orbits the camera around a target.<br>
Holding the `middle mouse button` and dragging the mouse pans the camera (left, right, up and down).<br>
Scrolling the `mouse wheel` dollys the camera forward and backward.<br>

Clicking the `left mouse button` selects an object in the scene that is under the cursor.<br>
Holding the `left mouse button` while hovering an object and then dragging the mouse,<br>
moves the object parallel to the screen.<br>

Holding `alt` highlights the currently selecte object by drawing a bounding box around it.<br>
While `alt` is still held, if the cursor hovers the selected object's bounding box,<br>
mouse interaction transforms the object along the plane of the bounding box that the cursor hovers on:<br>
Holding the `left mouse button` and dragging the mouse moves the object.<br>
Holding the `right mouse button` and dragging the mouse rotates the object.<br>
Holding the `middle mouse button` and dragging the mouse scales the object.<br>
<i>(`mouse wheel` interaction is disabled while `alt` is held)</i><br>