graph_concepts: graph_concepts.o digraph.o
	g++ graph_concepts.o digraph.o -o graph_concepts

graph_concepts.o: graph_concepts.cpp
	g++ graph_concepts.cpp -o graph_concepts.o -c -Wall

digraph.o: digraph.cpp
	g++ digraph.cpp -o digraph.o -c -Wall

clean:
	rm -f graph_concepts.o digraph.o graph_concepts