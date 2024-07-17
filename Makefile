errordetection.o: errordetection.cpp
	g++ -c errordetection.cpp

main: errordetection.o
	g++ -o main errordetection.o

all: main
	./main.exe

clean:
	rm -f *.o main.exe