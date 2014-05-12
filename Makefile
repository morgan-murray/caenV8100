CC = g++
LOCAL = -D NO_DEVICE  -D DEBUG_MAX -D DEBUG
DEV = -D DEBUG
OBJ= V8100.o V8100channel.o test.o
INC = -I libftdi -I/home/morgan/lib/libftdi
LIBDIRS = -L /usr/local/lib -L /home/morgan/lib/libftdi/build/x86_64
LIBS = -l ftd2xx
CFLAGS = -c -Wall

test: $(OBJ)
	$(CC) $(LIBDIRS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) $(DEV) $<

