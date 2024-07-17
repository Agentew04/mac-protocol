ERRORS = -Wall -Wextra

all: main
	@./main.exe

errordetection.o: errordetection.cpp
	g++ -c errordetection.cpp

main: errordetection.o main.cpp endpoint.o frame.o channel.o
	g++ -o main main.cpp errordetection.o endpoint.o frame.o channel.o

endpoint.o: Endpoint.cpp
	g++ -c endpoint.o Endpoint.cpp

frame.o: Frame.cpp
	g++ -c frame.o Frame.cpp

channel.o: Channel.cpp
	g++ -c channel.o Channel.cpp

clean:
	rm -f *.o main.exe