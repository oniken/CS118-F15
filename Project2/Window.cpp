#include <iostream>
#include <vector>
#include <queue>
#include <list>

#define MAX_PACKET_SIZE 1024
#define ACK_INCREMENT 1

using namespace std;

class Packet {
    public:

        bool isCorrupted() {
            return corrupted;
        }
    private:
        char* data;
        int seq;
        int ACK;
        bool request;
        bool ack;
        bool corrupted;
};

class Window {
    public:
        Window();
        Window(int cwnd);
        bool processPacket(Packet packet);

    private:
        int window_length;
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

Window::Window() {
    window_length = 20 * ACK_INCREMENT;
}

Window::Window(int cwnd): window_length(cwnd) {
}

bool Window::processPacket(Packet packet) {
    if (packet.isCorrupted()) {
        return false;
    }
   
    return true;
}
