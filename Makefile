CC = g++

CFLAGS = -std=c++17 -I./libs -I./include -I./points_generator

# OSによる条件分岐
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)  # macOS
	LDFLAGS = -framework OpenGL -framework GLUT -lGLEW -lglfw
else ifeq ($(UNAME_S), Linux)  # Linux
	LDFLAGS = -lGL -lglut -lGLEW -lglfw
else ifeq ($(OS), Windows_NT)  # Windows (assuming MinGW or MSYS2)
	# Windows環境用ライブラリの指定
	LDFLAGS = -lglew32 -lglfw3 -lopengl32 -lfreeglut
	CFLAGS += -I/mingw64/include
	LDFLAGS += -L/mingw64/lib
endif

TARGET = out
SOURCES = main.cpp libs/SimplexNoise.cpp $(wildcard points_generator/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
