#ifndef WINDOW
#define WINDOW

#include "PacketStream.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <list>
#include <ctime>
#include <unordered_map>
#include <cstdio>

#define MAX_PACKET_SIZE 1024
#define ACK_INCREMENT 1
#define TIMEOUT_SEC 20

using namespace std;

class Window {
    public:
        Window();
        Window(int cwnd);
        virtual bool processPacket(Packet packet) = 0;

    protected:
        int window_length;
        list<Packet> packets;
        struct comparator {
             bool operator()(int i, int j) {
                  return i > j;
             }
        };
        priority_queue<int, std::vector<int>, comparator> minHeap;
};

Window::Window(int cwnd): window_length(cwnd) {}


///////////////////////////////////////////////////////////////////////
// ServerWindow
///////////////////////////////////////////////////////////////////////

class ServerWindow: public Window {
    public:
        ServerWindow(int cwnd);
        int startStreaming(string filename);
        bool processPacket(Packet packet);
        bool sendPacket(Packet packet);
        void checkTimeout();

    private:
        list<Packet>::iterator lastPacket;
        unordered_map<int, clock_t> timers;
};

ServerWindow::ServerWindow(int cwnd): Window(cwnd) {
}






















#endif
