DEPENDS_DIR = $(PWD)/../../dependencies

CXX = g++
CXXFLAGS = -std=c++17 -g -I$(DEPENDS_DIR)/glew/include/ -I$(DEPENDS_DIR)/glfw/include/
LDFLAGS = -L$(DEPENDS_DIR)/glfw/lib -L$(DEPENDS_DIR)/glew/lib
LIBS = -lglfw -lGLEW -lGL

TARGET  = rectangle
SOURCES = main.cpp
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

clean:
	rm -f ${TARGET}
