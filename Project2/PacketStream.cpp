#include "PacketStream.h"
int Packet_Stream::initFile(char[] filename){
    ifstream f(buffer, ios::in|ios::binary|ios::ate);
            file=f;
            if(f.is_open()) {
                size=f.tellg();
                char* image=new char[size];
                f.seekg(0, ios::beg);
                f.read(image, size);
                f.close();
                float p=size/MAX_PACKET_SIZE;
                packetNumber=ceil(p);
                data= Packet[packetNumber];
                int i=0;
                while(i<packetNumber) {
                    if(i==packetNumber-1) {
                        int lastPacketSize=size%(MAX_PACKET_SIZE-sizeof(Packet));
                        char* tmp;
                        memcpy(tmp, image, lastPacketSize);
                        data[i].setData(tmp);
                        break;
                    }
                    else {
                        char* tmp;
                        memcpy(tmp, image, MAX_PACKET_SIZE-sizeof(Packet));
                        data[i].setData(tmp);
                        image+=(MAX_PACKET_SIZE-sizeof(Packet));
                    }
                    i++;
                }
                delete image[];
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
}
streampos PacketStream::getFileSize() {
    return size;
}
Packet* Packet_Stream::get(int x) {
    return &data[x];
}
Packet_Stream::getNumOfPacks() {
    return packetNumber;
}
int Packet_Stream::getSizeOfPacket(int i) {
    if(i==getNumOfPacks()-1) {
        return (getFileSize()%(MAX_PACKET_SIZE-sizeof(Packet)))+sizeof(Packet);
    }
    else
        return MAX_PACKET_SIZE;
}