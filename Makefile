

all:
	g++ src/main.cpp src/gameboy.cpp src/instructions.cpp src/graphics.cpp src/input.cpp -lSDL2  -g -march=native -o a

release:
	g++ -Isrc/include -Lsrc/lib -o a src/main.cpp src/gameboy.cpp src/mmu.cpp src/graphics.cpp \
-lmingw32 -lSDL2main -lSDL2 -O3 -march=native -flto -DNDEBUG -fno-exceptions -fno-rtti -s
