UNAME_S = $(shell uname -s)

CC = clang
CPP = clang++
CFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -Wno-newline-eof
CFLAGS += -Ilib/flgl/inc -Ilib/flgl/lib/glfw/include -Ilib/flgl/lib/glad/include -Ilib/flgl/lib/glm/ -Ilib/flgl/lib/stb
LDFLAGS = lib/flgl/lib/glad/src/glad.o lib/flgl/lib/glfw/src/libglfw3.a
LDFLAGS += -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa

SRC = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard lib/**/*.cpp) $(wildcard lib/flgl/src/*.cpp) $(wildcard lib/flgl/src/**/*.cpp) 
OBJ = $(SRC:.cpp=.o)
BIN = bin
TARGET = main.out

.PHONY: all clean dirs

all: dirs libs b

dirs:
	mkdir -p ./$(BIN)

libs:
	cd lib/flgl/lib/glad && $(CC) -o src/glad.o -Iinclude -c src/glad.c
	cd lib/flgl/lib/glfw && cmake . && make

b: $(BIN)/$(TARGET)

c: clean
	clear

cb: c b

r: $(BIN)/$(TARGET)
	./$(BIN)/$(TARGET)

cbr: c b r

$(BIN)/$(TARGET): $(OBJ) 
	mkdir -p ./$(BIN)
	$(CPP) -o $(BIN)/$(TARGET) $^ $(LDFLAGS)

%.o: %.cpp
	$(CPP) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf $(BIN) $(OBJ) 


