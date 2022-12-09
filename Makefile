CC = g++

.PHONY: all



msg.o: msg.cpp
	$(CC) -c msg.cpp

producer.o: producer.cpp
	$(CC) -c producer.cpp -g

consumer.o: consumer.cpp
	$(CC) -c consumer.cpp -g

consumer:  consumer.o msg.o
	$(CC) -o consumer consumer.o msg.o -g

producer: msg.o producer.o
	$(CC) -o producer producer.o msg.o -g

# mock_produce: producer
# 	./producer ZINC 10 0.6 500 & \
# 	./producer GOLD 10 0.5 500 & \
# 	./producer COPPER 10 0.8 700 & \
# 	./producer SILVER 10 0.4 100 & \
# 	./producer NICKEL 10 0.6 300 & \ 
# 	./producer LEAD 10 0.3 500 & \
# 	./producer COTTON 10 0.9 200 & \


clean:
	rm -f *.o *.out producer consumer 

