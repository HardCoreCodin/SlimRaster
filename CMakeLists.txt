cmake_minimum_required(VERSION 3.8)

project(SlimRaster_1_clipping_interpolatio)
add_executable(SlimRaster_1_clipping_interpolatio WIN32 src/examples/1_clipping_interpolation.c)
project(SlimRaster_2_normal_maps)
add_executable(SlimRaster_2_normal_maps WIN32 src/examples/2_normal_maps.c)
#project(SlimRaster_3_debug_shaders)
#add_executable(SlimRaster_3_debug_shaders WIN32 src/examples/1_clipping_interpolation.c)

project(obj2mesh)
add_executable(obj2mesh src/obj2mesh.c)

project(bmp2texture)
add_executable(bmp2texture src/bmp2texture.c)