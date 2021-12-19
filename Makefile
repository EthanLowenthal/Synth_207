CC = g++
CFLAGS = -std=c++17 -Ofast
 

main: src/main.cpp
	$(CC) $(CFLAGS) -o main src/main.cpp Midifile/libmidifile.a

.PHONY: main