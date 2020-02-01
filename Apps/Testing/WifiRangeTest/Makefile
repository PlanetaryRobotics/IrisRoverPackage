CC = g++
CFLAGS = -I/usr/include/libftdi1
DEPS =
OBJ_TX = transmitter.cpp 
OBJ_RX = receiver.cpp
LIBS = -lftdi1

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Transmitter:   $(OBJ_TX)
			   $(CC) -o $@ $^ $(CFLAGS) $(LIBS)

Receiver:   $(OBJ_RX)
			   $(CC) -o $@ $^ $(CFLAGS) $(LIBS)