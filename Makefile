CC = g++

CFLAGS = -framework OpenGL -framework GLUT -lGLEW -lglfw -std=c++17
TARGET = out
SRC = main.cpp

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -f $(TARGET)
