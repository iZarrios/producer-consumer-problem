CC = g++

.PHONY: all



msg.o: msg.cpp
	$(CC) -c msg.cpp

Queue.o: Queue.cpp
	$(CC) -c Queue.cpp

producer.o: producer.cpp
	$(CC) -c producer.cpp -g

consumer.o: consumer.cpp
	$(CC) -c consumer.cpp -g

consumer:  consumer.o msg.o Queue.o
	$(CC) -o consumer consumer.o msg.o Queue.o -g

producer: msg.o producer.o Queue.o
	$(CC) -o producer producer.o msg.o Queue.o -g

clean:
	rm -f *.o *.out producer consumer 

