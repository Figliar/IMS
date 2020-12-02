isabot: main.o DataCollector.o Worker.o
	g++ -Wall -w -g -o main main.o DataCollector.o Worker.o -lssl -lcrypto

main.o: main.cpp DataCollector.h Worker.h
	g++ -Wall -w -g -c main.cpp

DataCollector.o: DataCollector.cpp DataCollector.h
	g++ -Wall -g -c DataCollector.cpp

Worker.o: Worker.cpp Worker.h
	g++ -Wall -g -c Worker.cpp

clean :
	\rm -fr main main.o DataCollector.o Worker.o