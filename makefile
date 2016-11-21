CXX = g++
DBG = -g
LINKER = $(CXX)

# our macs use g++ 4.2.1, pasta uses g++ 4.1.2
# compilation will die because of a deprecated conversion from string
# constant to char* error
#CXXFLAGS = -O0 $(DBG) -Wall -Werror -Wno-unknown-pragmas -fno-strict-aliasing
CXXFLAGS = $(DBG) -Wall -fno-strict-aliasing -Wno-non-virtual-dtor

SOURCES:= messages.cpp\
	  MI_protocol.cpp\
	  MSI_protocol.cpp\
	  MESI_protocol.cpp\
	  MOSI_protocol.cpp\
	  MOESI_protocol.cpp\
	  MOESIF_protocol.cpp\
	  protocol.cpp

HEADERS:=$(patsubst %.cpp, %.h, $(SOURCES))
OBJECTS:=$(patsubst %.cpp, %.o, $(SOURCES))
DEPS:=$(patsubst %.cpp, %.d, $(SOURCES))

all: $(DEPS) sim
deps: $(DEPS)

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM $< > $@ 

include $(wildcard *.d)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o ${OUTOPT} $@

sim: $(DEPS) $(OBJECTS)
	ar r ../lib/libprotocols.a $(OBJECTS)
	ranlib ../lib/libprotocols.a

## cleaning
clean:
	-rm -rf *~ ../lib/libprotocols.a *.d *.o
