

This is a CPU-based scanline rasterizer. What this means is the scanline algorithm is run linearly
via the CPU, rather than parallelized via the GPU. This makes the algorithm simpler to write and implement,
but much slower than modern GPU rasterization algorithms. 

=== SETUP INSTRUCTIONS ===

1. Download SDL3 version 3.2.26 from: https://github.com/libsdl-org/SDL/releases/tag/release-3.2.26
   - Download the MinGW development libraries: SDL3-devel-3.2.26-mingw.tar.gz

2. Extract the SDL3 folder to this project directory:
   - Extract SDL3-3.2.26/ folder here (should contain x86_64-w64-mingw32/ subdirectory)

3. Copy the SDL3.dll to this project directory:
   - From SDL3-3.2.26/x86_64-w64-mingw32/bin/SDL3.dll
   - Copy to the same directory as the .cpp files

4. Compile the program:
   mingw32-make

5. Run the program:
   ./triangle_rasterizer.exe

The program has two modes: Default mode and custom mode. Default mode renders two hard-coded triangles.
Custom mode allows you to input vertices and colors to make your own triangles.

As of v1.0, the triangles do not have z-buffers. This means that the triangles don't have "depth."
The newest triangle rendered will always obscur any triangles "underneath" it.

Future work:
- Implementing Z-buffers so that the triangles have depth
- Implementing matrix transformations to allow the user to scale, rotate, and translate the triangles
- Allowing users to use any color they wish, rather than restricting them to the hard-coded colors
- Rewriting entirely to use the GPU rather than the CPU. This would likely be a separate project