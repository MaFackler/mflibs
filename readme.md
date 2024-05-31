# MF Libs

Single Header libraries/utilities for creating games etc.

* [mf.h](./src/mf.h) Common utilities
* [mf_file.h](./src/mf_file.h) File reading etcc
* [mf_font.h](./src/mf_font.h) Font parsing utilitys (uses stb\_truetype.h)
* [mf_image.h](./src/mf_image.h) Image parsing utilitys (uses stb\_image.h)
* [mf_math.h](./src/mf_math.h) Math utilities like Vector and Matrix implementation
* [mf_opengl.h](./src/mf_opengl.h) OpenGL helper functions for creating shaders etc.
* [mf_platform.h](./src/mf_platform.h) Platform utility for opening windows and read keyboard, mouse input
* [mf_platform_opengl.h](./src/mf_platform_opengl.h) Extension for Platform layer to create OpenGL context
* [mf_random.h](./src/mf_random.h) Random numbers
* [mf_string.h](./src/mf_string.h) String implementation
* [mf_test.h](./src/mf_test.h) Unit testing framework
* [mf_vector.h](./src/mf_vector.h) Dynamic Array Implementation

## Assets
Textures in asset folder are taken from [publicdomaintextures.com](www.publicdomaintextures.com).

## Compile and run

Build examples into `./build` folder.
Currently there are:
* Implementations of [learnopengl.com](www.learnopengl.com) with this library
* Example for Platform Window with and without OpenGL
* Ray tracer which produces a ppm file. [Ray Tracer In One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

```bash
make
```

Execute tests which are written with `mf_test.h`

```bash
make test
```



