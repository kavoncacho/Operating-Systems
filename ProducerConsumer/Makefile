CXX=g++ 
CXXFLAGS= -pthread -std=c++11 -g

rideshare : main.o io.o consumer_producer_functions.o
	$(CXX) $(CXXFLAGS) -o rideshare $^ -lpthread -lrt

main.o : consumer.h producer.h broker.h main.cpp consumer_producer_functions.h io.h

io.o : io.cpp io.h 

consumer_producer_functions.o : consumer_producer_functions.cpp consumer_producer_functions.h

clean :
	rm *.o