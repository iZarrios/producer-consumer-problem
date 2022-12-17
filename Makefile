CC = g++

.PHONY: clean



msg.o: msg.cpp
	$(CC) -c msg.cpp

Queue.o: Queue.cpp
	$(CC) -c Queue.cpp

producer.o: producer.cpp
	$(CC) -c producer.cpp -g

consumer.o: consumer.cpp
	$(CC) -c consumer.cpp -g

consumer: clean consumer.o msg.o 
	$(CC) -o consumer consumer.o msg.o -g

producer: clean msg.o producer.o 
	$(CC) -o producer producer.o msg.o -g

clean:
	rm -f *.o *.out producer consumer 

