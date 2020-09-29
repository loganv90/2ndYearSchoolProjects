#include <iostream>
#include <queue>
#include <unordered_map>
#include <list>
#include "digraph.h"
#include "wdigraph.h"
#include <unordered_set>
#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include "heap.h"
#include "dijkstra.h"
#include <stack>
#include <climits>
using namespace std;


// struct for storing coordinates of verticies
struct Point {
    long long lat;
    long long lon;
};


// returns a number of class double from between commas in a string
// changes nextind's value to be at the index of the beginning of the next integer
double grabD(string s, int index, int &nextind) {
    string temp;

    for (int i = index; i < s.size(); i++) {
        if (s[i] == ',') {
            nextind = i+1;
            break;
        }
        else {
            temp += s[i];
        }
    }

    return stod(temp);
}


// calculates the manhattan distance between two variables of class Point
long long manhattan(const Point& pt1, const Point& pt2) {
    return abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon);
}


// reads graph from filename and puts verticies with coords into points and puts edges into graph
void readGraph(string filename, WDigraph& graph, unordered_map<int,Point>& points) {
    ifstream fin;
    fin.open(filename);

    while (fin) {
        string currString;
        int nextind;
        getline(fin,currString);
        
        if (currString[0] == 'V') {
            // 2 is used here since the first integer is always at the second index of each line
            int vertice = grabD(currString, 2, nextind);
            long long lat = static_cast<long long>(grabD(currString, nextind, nextind)*100000);
            long long lon = static_cast<long long>(grabD(currString, nextind, nextind)*100000);

            Point coords = {lat,lon};
            points.insert({vertice,coords});
        }
        else if (currString[0] == 'E') {
            // 2 is used here since the first integer is always at the second index of each line
            int u = grabD(currString, 2, nextind);
            int v = grabD(currString, nextind, nextind);

            Point verticeu = points.find(u)->second;
            Point verticev = points.find(v)->second;
            long long cost = manhattan(verticeu, verticev);

            graph.addEdge(u, v, cost);
        }
    }

    fin.close();     
}


int main() {
    unordered_map<int,Point> points;
    WDigraph graph;

    // reads graph from edmonton text file
    readGraph("edmonton-roads-2.0.1.txt", graph, points);

    while(true) {
        char input;
        int startVertex = 0;
        int endVertex = 0;
        unordered_map<int,pair<int,long long>> tree;

        cin >> input;

        // if input is R then proceed by getting the start and end vertex
        if (input == 'R') {
            Point pointStart;
            cin >> pointStart.lat >> pointStart.lon;
            Point pointEnd;
            cin >> pointEnd.lat >> pointEnd.lon;

            // set to maximum possible value
            long long minDistStart = LLONG_MAX;
            long long minDistEnd = LLONG_MAX;

            // for every element i in points find the distance to the starting or ending inputted point and make the vertex 
            // with the closest distance to the inputted point the starting or ending vertex.
            for (const auto i : points) {
                if(manhattan(i.second, pointStart) < minDistStart){
                    startVertex = i.first;
                    minDistStart = manhattan(i.second, pointStart);
                }

                if(manhattan(i.second, pointEnd) < minDistEnd){
                    endVertex = i.first;
                    minDistEnd = manhattan(i.second, pointEnd);
                }
            }

            // builds minimum spanning tree from starting vertex
            dijkstra(graph, startVertex, tree);

            // if there is no path to end vertex output N 0<\n>
            if (tree.find(endVertex) == tree.end()) {
                cout << "N" << " " << 0 << endl;
            }
            else {
                stack<int> path;
                pair<int,pair<int,long long>> piece = *tree.find(endVertex);

                // pushes all verticies onto a stack from the ending vertex to the starting vertex
                while (piece.first != startVertex) {
                    path.push(piece.first);
                    piece = *tree.find(piece.second.first);
                }

                // pushes the starting vertex onto the stack
                path.push(startVertex);

                int size = path.size();

                cout << "N" << " " << size << endl;

                // outputs path from start vertex to end vertex and waits for acknowledgement between sending verticies
                for (int i=0; i < size; i++) {
                    cin >> input;

                    if (input == 'A') {
                        cout << "W " << points.find(path.top())->second.lat << " " << points.find(path.top())->second.lon << endl;
                        path.pop();
                    }
                }

                cout << "E" << endl;
            }
        }
    }

    return 0;
}
