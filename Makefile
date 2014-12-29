BIN = airlines
SRC = airlines.cc
OBJ = ${SRC:.cc=.o}

CC = g++
CXXFLAGS  = -g -Wall -O2 -std=c++11
LDFLAGS = -g 

all: ${BIN}

.cc.o:
	${CC} -c ${CXXFLAGS} $<

${BIN}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

run: ${BIN}
	./${BIN}

clean:
	rm -f *.o *.~ ${BIN}

.PHONY: all run clean
