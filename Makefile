# Makefile for Triangle Rasterizer with SDL3

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++11
SDL_INCLUDE = -I"SDL3-3.2.26/x86_64-w64-mingw32/include"
SDL_LIB = -L"SDL3-3.2.26/x86_64-w64-mingw32/lib"
SDL_LINK = -lSDL3

# Target executable name
TARGET = triangle_rasterizer.exe

# Source files
SOURCES = triangle_rasterizer.cpp

# Default target - runs when you type just "make"
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(SDL_INCLUDE) $(SDL_LIB) $(SDL_LINK) -o $(TARGET)
	@echo "Build complete! Run with: ./$(TARGET)"

# Run the program (requires SDL3.dll to be present)
run: $(TARGET)
	./$(TARGET)

# Copy SDL3.dll from the bin folder to current directory
setup-dll:
	copy "SDL3-3.2.26\x86_64-w64-mingw32\bin\SDL3.dll" .

# Clean up compiled files
clean:
	del /Q $(TARGET)

# Phony targets (not actual files)
.PHONY: all run clean setup-dll
