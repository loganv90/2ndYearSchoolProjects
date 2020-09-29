#include "dijkstra.h"

using namespace std;

// djiksra's algorithm for creating a minimum spanning tree
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int,pair<int,long long>>& tree) {
    BinaryHeap<pair<int,int>,int> events;
    events.insert({startVertex,startVertex}, 0);

    while (events.size() > 0) {
        pair<pair<int,int>,int> temp = {events.min().item, events.min().key};
        events.popMin();

        if (tree.find(temp.first.second) == tree.end()) {
            tree[temp.first.second] = {temp.first.first,temp.second};

            for(auto i = graph.neighbours(temp.first.second); i != graph.endIterator(temp.first.second); i++) {
                events.insert({temp.first.second,*i}, temp.second + graph.getCost(temp.first.second, *i));
            }
        }
    }
}