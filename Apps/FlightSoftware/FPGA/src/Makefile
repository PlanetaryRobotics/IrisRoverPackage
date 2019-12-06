EXES := local_dct

SRCS := main.cpp dct.cpp dct_hls.cpp util.cpp livingRoom.cpp decoder.cpp
OBJS := $(SRCS:.cpp=.o)

#OPTFLAGS := -O3 -DNDEBUG
#OPTFLAGS := -O2
OPTFLAGS := -g -Og

CXXFLAGS := -Wall

##########

CXXFLAGS += $(OPTFLAGS)

.PHONY: all
all: $(EXES)

local_dct: $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

.PHONY: clean
clean:
	$(RM) *~ $(OBJS) $(EXES)
