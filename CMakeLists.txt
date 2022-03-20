cmake_minimum_required(VERSION 3.8)

project(SlimRaster_1_normal_maps)
add_executable(SlimRaster_1_normal_maps WIN32 src/examples/1_normal_maps.c)

project(obj2mesh)
add_executable(obj2mesh src/obj2mesh.c)

project(bmp2texture)
add_executable(bmp2texture src/bmp2texture.c)