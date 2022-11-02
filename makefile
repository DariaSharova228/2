.PHONY: all debug
all: a.out

CFLAGS= -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal \
	-Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual \
	-Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format
%.o: %.cpp
	g++ -c -g $(CFLAGS) $< -o $@


sequence.o: sequence.cpp sequence.h
main.o: main.cpp sequence.h


a.out: sequence.o main.o
	g++ -pthread -o a.out sequence.o main.o

clean:
	rm -f *.out *.o
