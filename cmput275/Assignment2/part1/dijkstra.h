#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "wdigraph.h"
#include "digraph.h"
#include "heap.h"
#include <utility>
#include <unordered_map>

using namespace std;

// djiksra's algorithm for creating a minimum spanning tree
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int,pair<int,long long>>& tree);

#endif
