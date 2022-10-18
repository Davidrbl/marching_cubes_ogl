gcc -g shader.c -O2 -c -o shader.o

gcc -g gl.c -O2 -c -o gl.o

gcc -g main.c -O2 -c -o main.o

gcc ^
-g ^
shader.o ^
gl.o ^
main.o ^
-lglfw3 ^
-O2 ^
-o main.exe
