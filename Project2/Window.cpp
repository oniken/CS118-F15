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

// Send entire file, returns when entire file sent
int ServerWindow::startStreaming(string filename) {
    // TODO: Create PacketStream of filename
    // TODO: Push all packets from PacketStream to list<Packet> packets

    // Initialize and send first batch of Packets
    int i = 0;
    while (i++ < window_length && lastPacket != packets.end()) {
        sendPacket(*lastPacket);
        lastPacket++;
    }

    while (!packets.empty()) {
        Packet curr;
        // TODO: read a certain number of bytes and deserialize to Packet curr

        processPacket(curr);
        checkTimeout();
    }
    return 0;
}

bool ServerWindow::processPacket(Packet packet) {
    // Don't do anything if ACK packet is lost/corrupted
    if (packet.isLost() || packet.isCorrupted()) {
        return true;
    }
    cout << "Received ACK for packet number " << packet.ack << endl;
    minHeap.push(packet.seq);
    list<Packet>::iterator it = packets.begin();
    while (it != lastPacket) {
        bool entered = false;
        while (!minHeap.empty() && minHeap.top() == it->seq) {
            entered = true;
            minHeap.pop();
        }

        if (entered) {
            if (lastPacket != packets.end()) {
                sendPacket(*lastPacket);
                lastPacket++;
            }
            it++;
            packets.pop_front();
        }
        else {
            break;
        }
    }
    return true;
}

bool ServerWindow::sendPacket(Packet packet) {
    // TODO: Handle socket streaming here and serialization
    
    // Reset timer
    cout << "Sent packet with sequence number " << packet.seq << endl;
    timers[packet.seq] = clock();
    return true;
}

void ServerWindow::checkTimeout() {
    clock_t curr_time = clock();
    for (list<Packet>::iterator it = packets.begin(); it != lastPacket; it++) {
        double duration = (curr_time - timers[it->seq]) / (double) CLOCKS_PER_SEC;
        if (duration >= TIMEOUT_SEC) {
            sendPacket(*it);
        }
    }
}





































#endif
