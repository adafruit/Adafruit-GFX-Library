CPP=g++
CPPFLAGS=-Wall -g -I. -DARDUINO=100
OBJS=Adafruit_GFX.o RenderGFX.o render.o
LIBS=-lgd -lpng
TARGET=render
GFX_PATH=../

all: $(TARGET)

render: $(OBJS)
	$(CPP) -o $@ $^ $(LIBS)

%.o: %.c
	$(CPP) -c -o $@ $< $(CPPFLAGS)

Adafruit_GFX.o: $(GFX_PATH)/Adafruit_GFX.cpp
	$(CPP) -c -o $@ $< $(CPPFLAGS)

clean:
	rm -rf $(OBJS) $(TARGET)
