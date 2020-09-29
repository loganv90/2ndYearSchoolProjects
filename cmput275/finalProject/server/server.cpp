/*
CMPUT 275 EB1
Names: Andrew Culberson, Logan Vaughnan
ID #s: 1571644         , 1577489
Final Project: Build Your Own Tower Defense (BYOTD)
*/




#include <iostream>
#include <unordered_map>
#include "digraph.h"
#include "wdigraph.h"
#include <unordered_set>
#include <vector>
#include <string>
#include "dijkstra.h"
#include "serialport.h"
#include <stack>

using namespace std;

// initialize serial port
SerialPort Serial("/dev/ttyACM0");




// struct for storing coordinates of verticies
struct Point {
    long long lat;
    long long lon;
};




// This function creates a graph according to the clients input, excluding the nodes and connections for where walls have been placed
void createGraph(WDigraph& graph, unordered_map<int,Point>& points, unordered_set<int> wall) {
    int vertex = 0;

    // create points
    for (int y=0; y<26; y++) {
        for (int x=0; x<48; x++) {
            Point coords = {x,y};
            points.insert({vertex,coords});

            vertex++;
        }
    }


    vertex = 0;

    // add edges
    for (int y=0; y<26; y++) {
        for (int x=0; x<48; x++) {
            // if the vertex isn't a wall then make connections from vertex
            // The vertex below/above another vertex is that vertex +/- 48.
            // The vertex beside another vertex is that vertex +/- 1.
            // This is due to the dimensions of the grid
            if (wall.find(vertex) == wall.end()) {
                if (y==0) {
                    graph.addEdge(vertex, vertex+48, 1);
                }
                else if (y==25) {
                    graph.addEdge(vertex, vertex-48, 1);
                }
                else {
                    graph.addEdge(vertex, vertex+48, 1);
                    graph.addEdge(vertex, vertex-48, 1);
                }

                if (x==0) {
                    graph.addEdge(vertex, vertex+1, 1);
                }
                else if (x==47) {
                    graph.addEdge(vertex, vertex-1, 1);
                }
                else {
                    graph.addEdge(vertex, vertex+1, 1);
                    graph.addEdge(vertex, vertex-1, 1);
                }
            }

            vertex++;
        }
    }


    // cout stuff to show graph being created
    int q=0;
    for(auto i:points){
        cout << "VERTEX: " << i.second.lat << " " << i.second.lon << endl;
        cout << "VERTEXBELOW: " << points[i.first + 48].lat << " " << points[i.first + 48].lon << endl;
        q++;
        if(q == 10){
            break;
        } 
    }
}




// reads numbers from a string
void readStr(int toRead, int numbersRead[], string input) {
	int indexStart = 2;
    int indexNum = 0;

    for (int i=0; i<toRead; i++) {
        string temp = "";

        for (int i = indexStart; i<input.size(); i++){
            if(input[i] == ' ' or input[i] == '\n'){
                indexStart = i+1;
                break;
            }

            temp += input[i];
        }

        numbersRead[indexNum] = stoi(temp);
        indexNum++;
    }
}




// reads walls into the wall variable
void readWalls(bool& error, unordered_set<int>& wall) {
	string input;
	int numbersRead[2];
    error = false;

    // while loop until reads end message
    while (input[0] != 'E') {
    	// send acknowledgment
        input = Serial.readline(1000);
        cout << "INPUT: " << input;

        // if 'W' calculates wall, if 'E' end loop, otherwise set error to true
        if (input[0] == 'W') {
            Serial.writeline("A\n");
            cout << "A" << endl;

            // reads wall vertex into unordered map wall
            readStr(2, numbersRead, input);
            wall.insert(numbersRead[1]*48 + numbersRead[0]);
        }
        else if (input[0] != 'E') {
            cout << "ERROR: " << input;
            error = true;
            break;
        }
    }
}




// sends the path to the client
void sendPath(bool& error, int startVertex, int endVertex, unordered_map<int,Point> points, unordered_map<int,pair<int,long long>> tree) {
	stack<int> path;
    pair<int,pair<int,long long>> piece = *tree.find(endVertex);
    string writeString;
    string input;
    int size;
    error = false;

    // pushes all verticies onto a stack from the ending vertex to the starting vertex
    while (piece.first != startVertex) {
        path.push(piece.first);
        piece = *tree.find(piece.second.first);
    }
    path.push(startVertex);

    size = path.size();
    cout << "SIZE " << size << endl;

    // send the path size to the client
    Serial.writeline("N "+ to_string(size) + "\n");


    // outputs path from start vertex to end vertex and waits for acknowledgement between sending verticies
    for (int i=0; i < size; i++) {
        input = Serial.readline(1000);
        cout << input;

        writeString = "";

        if (input[0] == 'A') {
            writeString += "W ";
            writeString += to_string(points.find(path.top())->second.lat) + " ";
            writeString += to_string(points.find(path.top())->second.lon) + "\n";
            Serial.writeline(writeString);
            cout << writeString;
            path.pop();
        }
        else{
            // if timeout or incorrect character stop reading and go back to the beginning
            cout << "ERROR:" << input << endl;
            error = true;
            break;
        }
    }
}




int main() {
    string writeString;
    string input;
    int startVertex = 0;
    int endVertex = 1247;
    int numbersRead[4];
    bool error;


    while (true) {
        input = Serial.readline(1000);
        cout << "INITAL: " << input;

        // if 'X' isn't inputted loop restarts
        if (input[0] == 'X') {
        	// send acknowledgment
        	Serial.writeline("A\n");
            cout << "A" << endl;

        	// declaring variables
        	WDigraph graph;
            unordered_map<int,Point> points;
            unordered_map<int,pair<int,long long>> tree;
            unordered_set<int> wall;

            // read start and end vertex from input
            readStr(4, numbersRead, input);
            startVertex = numbersRead[1]*48 + numbersRead[0];
            endVertex = numbersRead[3]*48 + numbersRead[2];

            // reads walls into unordered map wall
            readWalls(error, wall);

            // if there was an error while reading restarts communication
            if(error){
                continue;
            }


            // graph is created omitting walls and points is made
            createGraph(graph, points, wall);

            // builds minimum spanning tree from starting vertex
            dijkstra(graph, startVertex, tree);
            cout << "DJIKSTRA DONE" << endl;


            // if there is no path to end vertex output N 0<\n>
            if (tree.find(endVertex) == tree.end()) {
                Serial.writeline("N 0\n");
                Serial.writeline("E\n");
                cout << "EMPTY" << endl;
            }
            else {
            	// sends the path of waypoints to the client
                sendPath(error, startVertex, endVertex, points, tree);

                // if there was an error while sending restarts communication
                if(error){
                    continue;
                }

                input = Serial.readline(1000);
                if(!error and input.find("A") != string::npos){
                    Serial.writeline("E\n");
                    cout << "E" << endl;
                }  
            }
        }
    }

    return 0;
}
