CXX = g++

CXXFLAGS_SERIAL = -Wall -std=c++17
CXXFLAGS_PARALLEL = -Wall -std=c++17 -fopenmp


TARGETS = serial parallel

SERIAL_SRC = serial.cpp
PARALLEL_SRC = parallel.cpp
NO_QUADTREE_SRC = parallel_no_qt.cpp

SERIAL_OBJ = $(SERIAL_SRC:.cpp=.o)
PARALLEL_OBJ = $(PARALLEL_SRC:.cpp=.o) quadtree.o
NO_QUADTREE_OBJ = $(NO_QUADTREE_SRC:.cpp=.o)

all: $(TARGETS)

s: serial
p: parallel
np: no_quadtree

serial: $(SERIAL_OBJ)
	$(CXX) $(CXXFLAGS_SERIAL) -o $@ $^

parallel: $(PARALLEL_OBJ)
	$(CXX) $(CXXFLAGS_PARALLEL) -o $@ $^

no_quadtree: $(NO_QUADTREE_OBJ)
	$(CXX) $(CXXFLAGS_PARALLEL) -o $@ $^

serial.o: serial.cpp
	$(CXX) $(CXXFLAGS_SERIAL) -c $< -o $@

parallel.o: parallel.cpp quadtree.h
	$(CXX) $(CXXFLAGS_PARALLEL) -c $< -o $@

quadtree.o: quadtree.cpp quadtree.h
	$(CXX) $(CXXFLAGS_SERIAL) -c $< -o $@

no_quadtree.o: parallel_no_qt.cpp
	$(CXX) $(CXXFLAGS_PARALLEL) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
