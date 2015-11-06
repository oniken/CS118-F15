#include <iostream>
#include <queue>
#include <list>

using namespace std;

class Packet {
    public:

    private:
        int data;

};

class Window {
    public:

    private:
        int size;
        list<Packet> packets;
        // 
        struct comparator {
             bool operator()(int i, int j) {
                  return i > j;
                   }
        };
        // sequence number
        priority_queue<int, std::vector<int>, comparator> minHeap;

};
