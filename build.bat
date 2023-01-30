mkdir build/src
mkdir build/libs

gcc -g src/shader.c -O2 -c -o build/src/shader.o

gcc -g libs/gl.c -O2 -c -o build/libs/gl.o

gcc -g src/main.c -O2 -c -o build/src/main.o

gcc -g src/bmp_imp.c -O2 -c -o build/src/bmp_imp.o

gcc -g src/marching_cubes_mesh_gen.c -O2 -c -o build/src/marching_cubes_mesh_gen.o

gcc -g src/voxel_mesh_gen.c -O2 -c -o build/src/voxel_mesh_gen.o

gcc ^
-g ^
build/src/shader.o ^
build/libs/gl.o ^
build/src/main.o ^
build/src/bmp_imp.o ^
build/src/marching_cubes_mesh_gen.o ^
build/src/voxel_mesh_gen.o ^
-lglfw3 ^
-O2 ^
-o main.exe
