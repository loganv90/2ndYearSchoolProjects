server: dijkstra.o server.o digraph.o serialport.o
	g++ dijkstra.o server.o digraph.o serialport.o -o server

digraph.o: digraph.cpp
	g++ digraph.cpp -c

server.o: server.cpp
	g++ server.cpp -c

dijkstra.o: dijkstra.cpp
	g++ dijkstra.cpp -c

serialport.o: serialport.cpp
	g++ serialport.cpp -c

clean:
	rm -f dijkstra.o server.o digraph.o serialport.o server