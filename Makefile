all:
	gcc `pkg-config --cflags sdl2` -Wall grapher.c `pkg-config --libs sdl2` -lm
