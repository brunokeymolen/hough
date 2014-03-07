# (C)2013, Bruno Keymolen
# http://www.keymolen.com
# http://www.keymolen.com/2013/05/hough-transformation-c-implementation.html
CXX=g++
CC=gcc
#OPTFLAGS=-g3 -ggdb -O0
OPTFLAGS=-g0 -O3
CXXFLAGS=-Wall -I. -I/usr/local/include $(OPTFLAGS)
CFLAGS=-Wall $(OPTFLAGS)
LDFLAGS= -L/usr/local/lib $(OPTFLAGS)

LDFLAGS+= -L/opt/keymolen/opencv/2.4.8/lib
CXXFLAGS+= -I/opt/keymolen/opencv/2.4.8/include/

LDFLAGS+= -lopencv_highgui -lopencv_core -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_imgproc -lopencv_objdetect -lopencv_ts

SRC = 	main.o hough.o hough_circle.o
	
all: hough 

hough: $(SRC) $(MODULES)
	$(CXX) $(MODULES) $(SRC) $(LDFLAGS) -o hough

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o hough

PREFIX ?= /usr

install: all
	install -d $(PREFIX)/bin
	install hough  $(PREFIX)/bin

.PHONY: clean all hough install
