For the program to work, ensure you have the SDL3.dll in the same directory as triangle_rasterizer.cpp.
The program has two modes: Default mode and custom mode. Default mode renders two hard-coded triangles.
Custom mode allows you to input vertices and colors and make your own triangles.

As of v1.0, the triangles do not have z-buffers. This means that the triangles don't have "depth."
The newest triangle rendered will always obscur any triangles "underneath" it.