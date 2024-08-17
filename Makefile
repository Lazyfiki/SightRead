FLAGS = -lSDL2 -lSDL2_ttf

CC = g++
SRC = main.cpp util.cpp

all: SightRead

SightRead: ${SRC}
	${CC} ${SRC} -o $@ ${FLAGS}
