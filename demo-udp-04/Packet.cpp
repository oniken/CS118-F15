#include "Packet.h"

Packet::Packet(char* stuff) {
    strcpy(data, stuff);
}
Packet::Packet() {
    Corrupted=false;
    ACK=0;
    seq=0;
    Ack=false;
    bzero(data, PAYLOAD_SIZE + 1);
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
    //TODO
    Corrupted = false;
    return;
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
    strcpy(data, c);
}
char* Packet::getData() {
    return data;
}
void Packet::setAck(int x) {
    ACK=x;
}
void Packet::setSeq(int x) {
    seq=x;
}
