CC = g++
CFLAGS =
DEPS = main.hpp
OBJ = main.cpp 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

WifiRangeTest: $(OBJ)
			   $(CC) -o $@ $^ $(CFLAGS)