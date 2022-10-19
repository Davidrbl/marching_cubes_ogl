gcc -g shader.c -O2 -c -o obj/shader.o

gcc -g gl.c -O2 -c -o obj/gl.o

gcc -g main.c -O2 -c -o obj/main.o

gcc -g bmp_imp.c -O2 -c -o obj/bmp_imp.o

gcc -g marching_cubes_mesh_gen.c -O2 -c -o obj/marching_cubes_mesh_gen.o

gcc -g voxel_mesh_gen.c -O2 -c -o obj/voxel_mesh_gen.o

gcc ^
-g ^
obj/shader.o ^
obj/gl.o ^
obj/main.o ^
obj/bmp_imp.o ^
obj/marching_cubes_mesh_gen.o ^
obj/voxel_mesh_gen.o ^
-lglfw3 ^
-O2 ^
-o main.exe
