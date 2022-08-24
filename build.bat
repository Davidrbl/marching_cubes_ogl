gcc shader.c -c -o shader.o

gcc gl.c -c -o gl.o

gcc main.c -c -o main.o

gcc ^
shader.o ^
gl.o ^
main.o ^
glfw3.dll ^
-O2 ^
-o main.exe
