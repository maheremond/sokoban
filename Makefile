# Sokoban Simple
CC = gcc
CFLAGS = -Wall -std=c99
SDL3_DIR = sdl2/SDL3-3.2.24/x86_64-w64-mingw32

# Compilation
sokoban: main_graphics.c board.c graphics_sdl3.c sokoban.h graphics.h
	$(CC) $(CFLAGS) -I"$(SDL3_DIR)/include" -o sokoban main_graphics.c board.c graphics_sdl3.c -L"$(SDL3_DIR)/lib" -lSDL3
	copy "$(SDL3_DIR)\bin\SDL3.dll" . >nul 2>&1

clean:
	del sokoban.exe 2>nul

.PHONY: clean