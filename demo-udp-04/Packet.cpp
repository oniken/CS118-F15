#include "Packet.h"

Packet::Packet(char* stuff) {
    packet tmp;
    memcpy(&tmp,stuff,sizeof(tmp));
    strcpy(data, tmp.data);
    strcpy(seq, tmp.seq);
    Corrupted=tmp.Corrupted;
}
Packet::Packet() {
    Corrupted=false;
    //ACK=0;
    seq[0]='0';
    seq[1]=0;
    //Ack=false;
    bzero(data, PAYLOAD_SIZE + 1);
}
// void Packet::setIsAck(bool x) {
//     Ack=x;
// }
// bool Packet::isAck() {
//     return Ack;
// }
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

// void Packet::setSeqAckNum(int m_seq, int m_ack) {
//     setSeq(m_seq);
//     ACK=m_ack;
// }
// int Packet::getACK() {
//     return ACK;
// }
int Packet::getSeq() {
    return atoi(seq);
}
void Packet::setData(char* c) {
    memcpy(&data[0], &c[0], PAYLOAD_SIZE);
}
char* Packet::getData() {
    return data;
}
// void Packet::setAck(int x) {
//     ACK=x;
// }
void Packet::setSeq(int x) {
    stringstream convert;
    convert<<x;
    string c=convert.str();
    for(int i=0;i<c.length();i++)
        seq[i]=c[i];
    seq[c.length()]=0;
}
