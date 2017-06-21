## Pectin - A base for your jam

Pectin is a clump of base code for game jams. The focus is grid-based 2d games. It uses SDL2, Chipmunk 2d and OpenGL, and renders using a sort of "point sprite" method where sprites are extruded, rotated and scaled in a geometry shader.

To compile, this project expects GLAD (in `deps/glad`, and stb_lib (in `deps/stb`). It will try to find SDL. It links `libchipmunk` statically.