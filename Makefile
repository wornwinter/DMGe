CC=g++
DEFS=-D_DEBUG
CFLAGS=-c -g -std=c++11 -Wall -fpermissive -I include `wx-config --cppflags` $(DEFS)
LDFLAGS= `wx-config --libs --gl-libs` -lGL -lglut -lboost_system -lboost_thread
SOURCES=src/main.cpp src/app.cpp src/canvas.cpp src/Cartridge.cpp src/debug.cpp src/Debugger.cpp src/DMGCPU.cpp src/GameBoy.cpp src/GPU.cpp src/Input.cpp src/mainFrame.cpp src/MMU.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=DMGe.bin

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm src/*.o $(EXECUTABLE)
