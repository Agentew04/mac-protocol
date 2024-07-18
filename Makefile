ERRORS = -Wall -Wextra

all: main
	@./main.exe

errordetection.o: errordetection.cpp
	g++ -c errordetection.cpp -o errordetection.o

main: errordetection.o main.cpp endpoint.o frame.o channel.o
	g++ -o main main.cpp errordetection.o endpoint.o frame.o channel.o

endpoint.o: Endpoint.cpp
	g++ -c -o endpoint.o Endpoint.cpp

frame.o: Frame.cpp
	g++ -c -o frame.o Frame.cpp

channel.o: Channel.cpp
	g++ -c -o channel.o Channel.cpp

clean:
	rm -f *.o main.exe