# Source and target
SRCS = main.cpp
TARGET = raylib_app

# Add the CS3113 library if it exists
ifeq ($(wildcard CS3113/cs3113.cpp),CS3113/cs3113.cpp)
    SRCS += CS3113/cs3113.cpp
endif

# Add the Entity library if it exists
ifeq ($(wildcard CS3113/Entity.cpp),CS3113/Entity.cpp)
    SRCS += CS3113/Entity.cpp
endif

# Add the Paddle library if it exists
ifeq ($(wildcard CS3113/Paddle.cpp),CS3113/Paddle.cpp)
    SRCS += CS3113/Paddle.cpp
endif

# Add the Ball library if it exists
ifeq ($(wildcard CS3113/Ball.cpp),CS3113/Ball.cpp)
    SRCS += CS3113/Ball.cpp
endif

# OS detection (macOS = Darwin, Windows via MinGW = MINGW*)
UNAME_S := $(shell uname -s)

# Default values
CXX = g++
CXXFLAGS = -std=c++11

# Raylib configuration using pkg-config
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

ifeq ($(UNAME_S), Darwin)
    # macOS configuration
    CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
    LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    EXEC = ./$(TARGET)
else ifneq (,$(filter MINGW% MSYS% CYGWIN%,$(UNAME_S)))
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)
else
    # Linux/WSL fallback
    CXXFLAGS +=
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    EXEC = ./$(TARGET)
endif

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	@if [ -f "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -f "$(TARGET).exe" ]; then rm -f $(TARGET).exe; fi

# Run rule
run: $(TARGET)
	$(EXEC)