# I get linking errors with g++...
CXX = clang++-9

CXXFLAGS += -Wall -Wextra -pedantic -I include/ -MMD -MP -O3 #-g
LDLIBS += -lpthread

ISPCFLAGS += --target=avx2-i32x16

all: src/main

src/main: src/main.o src/tasksys.o src/mandelbrot_ispc.o src/mandelbrot_task_ispc.o
	$(CXX) $(LDLIBS) $^ -o $@

src/main.o: src/main.cpp src/mandelbrot_ispc.o src/mandelbrot_task_ispc.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

src/tasksys.o: src/tasksys.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

src/mandelbrot_ispc.o: src/mandelbrot_ispc.ispc
	ispc $(ISPCFLAGS) $< -o $@ -h $(patsubst src/%.ispc,include/%.h,$<)

src/mandelbrot_task_ispc.o: src/mandelbrot_task_ispc.ispc
	ispc $(ISPCFLAGS) $< -o $@ -h $(patsubst src/%.ispc,include/%.h,$<)

.PHONY: clean dis

dis: src/main
	objdump -d -Mintel,i8086 $< | c++filt 

clean:
	rm -f src/main
	rm -f src/*.o
	rm -f src/*.d

-include $(wildcard src/*.d)
