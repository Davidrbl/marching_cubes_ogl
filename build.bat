::gcc shader.c -O2 -c -o shader.o

::gcc gl.c -O2 -c -o gl.o

gcc main.c -O2 -c -o main.o

gcc ^
shader.o ^
gl.o ^
main.o ^
glfw3.dll ^
-O2 ^
-o main.exe
