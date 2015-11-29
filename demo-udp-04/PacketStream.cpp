#include "PacketStream.h"

int Packet_Stream::initFile(char* filename){
            ifstream f(filename, ios::in|ios::binary|ios::ate);

            if(f.is_open()) {
                size=f.tellg();
                char* image=new char[size];
                f.seekg(0, ios::beg);
                f.read(image, size);
                f.close();
                float p=size/MAX_PACKET_SIZE;
                packetNumber=ceil(p);
                data= new Packet[packetNumber];
                int i=0;
                while(i<packetNumber) {
                    if(i==packetNumber-1) {
                        int lastPacketSize=size%(MAX_PACKET_SIZE-sizeof(Packet));
                        char* tmp;
                        memcpy(tmp, image, lastPacketSize);
                        data[i].setData(tmp);
                        data[i].setSeqAckNum(i, -1);
                        break;
                    }
                    else {
                        char* tmp;
                        memcpy(tmp, image, MAX_PACKET_SIZE-sizeof(Packet));
                        data[i].setData(tmp);
                        data[i].setSeqAckNum(i, -1);
                        image+=(MAX_PACKET_SIZE-sizeof(Packet));
                    }
                    i++;
                }
                flg=true;
                delete image;
                return 0;
            }
            else
                return -1;
}
Packet_Stream::Packet_Stream(int startingseq) {
            start_seq=startingseq;
            max_seq=0;
}
Packet_Stream::Packet_Stream(){
    start_seq=0;
    max_seq=0;
    flg=false;
}
streampos Packet_Stream::getFileSize() {
    return size;
}
Packet* Packet_Stream::get(int x) {
    if(x>=getNumOfPacks())
        return NULL;
    return &data[x];
}
int Packet_Stream::getNumOfPacks() {
    return packetNumber;
}
int Packet_Stream::getSizeOfPacket(int i) {
    if(i==getNumOfPacks()-1) {
        return (getFileSize()%(MAX_PACKET_SIZE-sizeof(Packet)))+sizeof(Packet);
    }
    else
        return MAX_PACKET_SIZE;
}
int Packet_Stream::setDataSize(int s) {
    if(!flg) {
        packetNumber=s;
        return 0;
    }
    else
        return -1;
}
int Packet_Stream::insert(Packet p, int pos) {
    if(pos>=getNumOfPacks()) {
        return -1;
    }
    data[pos]=p;
    return 0;
}