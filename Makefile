CXX = g++
CFLAGS = -Wall -Wextra -Werror -std=c++17
TARGET = transpiler
OBJFILES = main.o parser.o table.o

${TARGET}: ${OBJFILES}
	${CXX} ${OBJFILES} -o ${TARGET}

main.o: main.cpp
	${CXX} ${CFLAGS} -c main.cpp -o main.o

parser.o: parser.cpp parser.h table.o
	${CXX} ${CFLAGS} -c parser.cpp -o parser.o

table.o: table.cpp table.h
	${CXX} ${CFLAGS} -c table.cpp -o table.o

clean:
	rm -f ${TARGET} ${OBJFILES}
