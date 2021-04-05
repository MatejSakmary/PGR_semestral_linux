CC = g++
CFLAGS = -lglfw -lGL -lX11 -lXrandr -lXi -ldl
INCLUDES = vendor/

gl: src/main.cpp 
	$(CC) $(CFLAGS) -I$(INCLUDES) src/main.cpp vendor/glad/glad.c -o semestral
