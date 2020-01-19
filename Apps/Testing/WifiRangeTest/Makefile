CC = g++
CFLAGS =
DEPS =
OBJ_TX = transmitter.cpp 
OBJ_RX = receiver.cpp
LIBS =

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Transmitter:   $(OBJ_TX)
			   $(CC) -o $@ $^ $(CFLAGS) $(LIBS)

Receiver:   $(OBJ_RX)
			   $(CC) -o $@ $^ $(CFLAGS) $(LIBS)