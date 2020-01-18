CC = g++
CFLAGS =
DEPS =
OBJ_TX = transmitter.cpp 
OBJ_RX = receiver.cpp

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Transmitter:   $(OBJ_TX)
			   $(CC) -o $@ $^ $(CFLAGS)

Receiver:   $(OBJ_RX)
			   $(CC) -o $@ $^ $(CFLAGS)