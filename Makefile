CXX = g++

CXXFLAGS_SERIAL = -Wall -std=c++17
CXXFLAGS_PARALLEL = -Wall -std=c++17 -fopenmp

TARGETS = serial parallel

SERIAL_SRC = serial.cpp
PARALLEL_SRC = parallel.cpp

SERIAL_OBJ = $(SERIAL_SRC:.cpp=.o)
PARALLEL_OBJ = $(PARALLEL_SRC:.cpp=.o)

all: $(TARGETS)

s: serial
p: parallel

serial: $(SERIAL_OBJ)
	$(CXX) $(CXXFLAGS_SERIAL) -o $@ $^

parallel: $(PARALLEL_OBJ)
	$(CXX) $(CXXFLAGS_PARALLEL) -o $@ $^

serial.o: serial.cpp
	$(CXX) $(CXXFLAGS_SERIAL) -c $< -o $@

parallel.o: parallel.cpp
	$(CXX) $(CXXFLAGS_PARALLEL) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
