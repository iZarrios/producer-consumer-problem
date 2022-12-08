CC = g++

.PHONY: all



msg.o: msg.cpp
	$(CC) -c msg.cpp

producer.o: producer.cpp
	$(CC) -c producer.cpp

consumer.o: consumer.cpp
	$(CC) -c consumer.cpp

consumer: msg.o consumer.o
	$(CC) -o consumer consumer.o msg.o

producer: msg.o producer.o
	$(CC) -o producer producer.o msg.o
clean:
	rm -f *.o *.out producer consumer 

