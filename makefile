
cacheSim: cacheSim.cpp
	g++ -g -Wall -std=c++11 -o cacheSim cacheSim.cpp 

.PHONY: clean
clean:
	rm -f *.o
	rm -f cacheSim
