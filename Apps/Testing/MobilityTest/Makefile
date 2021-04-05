CC = g++
CFLAGS =
DEPS =
OBJ_TX = mobility.cpp 
OBJ_RX = mobility.cpp
LIBS =

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Mobility:   $(OBJ_TX)
			   $(CC) -o $@ $^ $(CFLAGS) $(LIBS)