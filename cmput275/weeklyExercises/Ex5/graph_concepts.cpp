// A template for the Graph Concepts Exercise in C++.

#include <iostream>
#include "digraph.h"
#include <fstream>
#include <string>
#include <queue>

using namespace std;


//provided bredthFirstSearch algorithm
unordered_map<int, int> breadthFirstSearch(const Digraph& graph, int startVertex) {
  unordered_map<int, int> searchTree; // map each vertex to its predecessor

  searchTree[startVertex] = -1;

  queue<int> q;
  q.push(startVertex);

  while (!q.empty()) {
    int v = q.front();
    q.pop();

    for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
      if (searchTree.find(*iter) == searchTree.end()) {
        searchTree[*iter] = v;
        q.push(*iter);
      }
    }
  }

  return searchTree;
}


//calculates the number of components in a given graph. Returns this number
int count_components(Digraph g) {
	vector<int> verts = g.vertices();
	int count = 0;
	unordered_map<int, int> map;
	unordered_map<int, int> newMap;

  //iterates through a vector containing all the vertices of the given graph and uses
  //the breadthFirstSearch algorithm to find all the connected components to each vertex
  //that has not yet been seen. Once a vertex is found by the algorithm, it is stored in
  //an unordered map and, for every group of vertices found, the counter is increased by 1.
	for (auto element : verts) {
		if (map.find(element) == map.end()) {
			newMap = breadthFirstSearch(g, element);
			map.insert(newMap.begin(), newMap.end());

			count++;
		}
	}

    return count;
}


//reads the graph from the text file and returns the intormation in a Digraph variable
Digraph read_city_graph_undirected(char filename[]) {
  //open text file
	ifstream fin;
	fin.open(filename);

	string line;
	Digraph graph;

  //while the end of the text file is not reached, information will be grabbed from
  //the text file and will be added to the graph as an edge or a vertex depending on
  //what the information is describing.
	while (fin) {
		getline(fin, line);

		int firstComma = line.find(',');
		int secondComma = line.find(',', firstComma+1);
		int thirdComma = line.find(',', secondComma+1);

		if (line[0] == 'V') {
      //this line of code takes a substring from a line of text and converts it to
      //an integer. In this case, the substring is between the first and second commas
      //in the line of text
			int ID = stoi(line.substr(firstComma+1, secondComma-(firstComma+1)));

			graph.addVertex(ID);
		}
		else if (line[0] == 'E') {
			int start = stoi(line.substr(firstComma+1, secondComma-(firstComma+1)));
			int end = stoi(line.substr(secondComma+1, thirdComma-(secondComma+1)));

			graph.addEdge(start, end);
			graph.addEdge(end, start);
		}
	}

  //close text file
	fin.close();

	return graph;
}


int main(int argc, char *argv[]) {
  //checks to make sure there is an extra argument (for text file) when file is executed
	if (argc == 2) {
		Digraph directionalGraph = read_city_graph_undirected(argv[1]);
		int components = count_components(directionalGraph);

		cout << components << endl;
	}

    return 0;
}