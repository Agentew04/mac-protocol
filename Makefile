ERRORS = -Wall -Wextra

all: main
	@./main.exe

errordetection.o: errordetection.cpp
	g++ -c errordetection.cpp

endpoint.o: Endpoint.cpp
	g++ -c Endpoint.cpp -o endpoint.o

main: errordetection.o main.cpp endpoint.o frame.o channel.o
	g++ -o main main.cpp errordetection.o endpoint.o frame.o channel.o $(ERRORS)

frame.o: Frame.cpp
	g++ -c Frame.cpp -o frame.o

channel.o: Channel.cpp
	g++ -c Channel.cpp -o channel.o

clean:
	rm -f *.o main.exe