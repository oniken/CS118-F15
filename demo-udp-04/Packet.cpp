#include "Packet.h"
Packet::~Packet() {}
Packet::Packet(char* stuff) {
    packet tmp;
    memcpy(&tmp,stuff,sizeof(tmp));
    memcpy(&data[0], &tmp.data[0], PAYLOAD_SIZE);
    memcpy(&seq[0], &tmp.seq[0], 9);
    Corrupted=tmp.Corrupted;
    Lost=tmp.Lost;
}
Packet::Packet() {
    Lost=false;
    Corrupted=false;
    seq[0]='0';
    seq[1]=0;
    bzero(data, PAYLOAD_SIZE + 1);
}
bool Packet::isCorrupted() {
    return Corrupted;
}
bool Packet::isLost() {
    return Lost;
}
void Packet::setIsCorrupted(float x) {
    srand(time(NULL));
    float r=((double)rand())/((double)RAND_MAX);
    if(x<=r) {
        Corrupted=false;
    }
    else
        Corrupted=true;
}
void Packet::setIsLost(float x) {
    srand(time(NULL));
    float r=((double)rand())/((double)RAND_MAX);
    if(x<=r) {
        Lost=false;
    }
    else
        Lost=true;
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
