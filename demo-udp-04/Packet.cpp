#include "Packet.h"
Packet::~Packet() {}
Packet::Packet(char* stuff) {
    packet tmp;
    memcpy(&tmp,stuff,sizeof(tmp));
    memcpy(&data[0], &tmp.data[0], PAYLOAD_SIZE);
    memcpy(&seq[0], &tmp.seq[0], 9);
    Corrupted=tmp.Corrupted;
}
Packet::Packet() {
    Corrupted=false;
    seq[0]='0';
    seq[1]=0;
    bzero(data, PAYLOAD_SIZE + 1);
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
int Packet::getSeq() {
    return atoi(seq);
}
void Packet::setData(char* c) {
    memcpy(&data[0], &c[0], PAYLOAD_SIZE);
}
char* Packet::getData() {
    return data;
}
void Packet::setSeq(int x) {
    stringstream convert;
    convert<<x;
    string c=convert.str();
    for(int i=0;i<c.length();i++)
        seq[i]=c[i];
    seq[c.length()]=0;
}
