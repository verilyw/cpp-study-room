CC := g++
CFLAGS := -std=c++20

all: math-lib

math-lib: src/math.cpp src/main.cpp
	$(CC) $(CFLAGS) -o math-lib src/main.cpp src/math.cpp

.PHONY: clean
clean:
	rm -f *.o *.s math-lib