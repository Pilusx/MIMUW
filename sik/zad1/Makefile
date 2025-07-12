TARGET: echo-server

CC	= g++
CXXFLAGS = -std=c++17
CFLAGS	= -Wall -O2
LFLAGS	= -Wall

echo-server.o err.o: err.h

echo-server: echo-server.o err.o
	$(CC) $(LFLAGS) $^ -o $@

.PHONY: clean TARGET
clean:
	rm -f echo-server *.o *~ *.bak
