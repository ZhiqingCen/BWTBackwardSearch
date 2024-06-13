all:
	g++ -o bwtsearch bwtsearch.cpp -std=c++11

clean:
	rm -f bwtsearch
