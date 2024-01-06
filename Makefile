CC = g++

CFLAGS = -std=c++17 -I./libs -I./include -I./points_generator
LDFLAGS = -framework OpenGL -framework GLUT -lGLEW -lglfw
TARGET = out
SOURCES = main.cpp libs/SimplexNoise.cpp $(wildcard points_generator/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
