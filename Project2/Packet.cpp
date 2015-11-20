#include "Packet.h"

Packet::Packet(char* stuff) {
    data=stuff;
}
Packet::Packet() {
    setIsAck(false);
    setIsCorrupted(1);
    setSeqAckNum(0,0);
}
Packet::~Packet() {
    delete[] data;
}
void Packet::setIsAck(bool x) {
    Ack=x;
}
bool Packet::isAck() {
    return Ack;
}
bool Packet::isCorrupted() {
    return Corrupted;
}
void Packet::setIsCorrupted(float x) {
    srand(time(NULL));
    if(x>rand()) {
        Corrupted=false;
    }
    else
        Corrupted=true;
}

void Packet::setSeqAckNum(int m_seq, int m_ack) {
    seq=m_seq;
    ACK=m_ack;
}
int Packet::getACK() {
    return ACK;
}
int Packet::getSeq() {
    return seq;
}
void Packet::setData(char* c) {
    data=c;
}
