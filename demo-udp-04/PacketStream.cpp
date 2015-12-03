#include "PacketStream.h"

int PacketStream::initFile(char* filename){
            //ifstream f(filename, ios::in|ios::binary|ios::ate);
            FILE* f = fopen(filename, "r");
            if(f==NULL)
                return -1;
            fseek(f, 0L, SEEK_END);
            long size = ftell(f);
            fseek(f, 0L, SEEK_SET);
            // if(f.is_open()) {
            //     size=f.tellg();
                char image[size];
                // f.seekg(0, ios::beg);
                // f.read(image, size);
                // f.close();
                fread(image, sizeof(char), size,f);
                fclose(f);
                fileSize=size;
                double p=fileSize/MAX_PACKET_SIZE;
                packetNumber=ceil(p);
                long lastPacketSize=size%(MAX_PACKET_SIZE);
                packetNumber=(lastPacketSize)?packetNumber+1:packetNumber;
                data= new Packet[packetNumber];
                long i=0;
                long it=0;
                while(i<packetNumber) {
                    if(i==packetNumber-1) {
                        char tmp[MAX_PACKET_SIZE];
                        // long j=0;
                        // while(j<lastPacketSize) {
                        //     tmp[j]=image[it];
                        //     j++;
                        //     it++;
                        // }
                        // tmp[j]=0;
                        strncpy(tmp, image+it, lastPacketSize);
                        //tmp[lastPacketSize]=0;
                        data[i].setData(tmp);
                        data[i].setSeq(i);
                        break;
                    }
                    else {
                        char tmp[MAX_PACKET_SIZE];
                        // long j=0;
                        // while(j<(MAX_PACKET_SIZE)) {
                        //     tmp[j]=image[it];
                        //     j++;
                        //     it++;
                        // }
                        // tmp[j]=0;
                        strncpy(tmp, image+it, MAX_PACKET_SIZE);
                        //tmp[MAX_PACKET_SIZE]=0;
                        data[i].setData(tmp);
                        data[i].setSeq(i);
                        it+=MAX_PACKET_SIZE;
                    }
                    i++;
                }
                flg=true;
                //if (image)
                   // delete image;
                return 0;
            // }
            // else
            //     return -1;
}
PacketStream::PacketStream(int startingseq) {
            start_seq=startingseq;
            max_seq=0;
}
PacketStream::~PacketStream() 
{
}
PacketStream::PacketStream(){
    start_seq=0;
    max_seq=0;
    flg=false;
}
long PacketStream::getFileSize() {
    return fileSize;
}
Packet PacketStream::get(int x) {
    return data[x];
}
int PacketStream::getNumOfPacks() {
    return packetNumber;
}
int PacketStream::getSizeOfPacket(int i) {
    if(i==getNumOfPacks()-1) {
        return (getFileSize()%(MAX_PACKET_SIZE-sizeof(Packet)))+sizeof(Packet);
    }
    else
        return MAX_PACKET_SIZE;
}
int PacketStream::setDataSize(int s) {
    if(!flg) {
        data = new Packet[s];
        packetNumber=s;
        return 0;
    }
    else
        return -1;
}
int PacketStream::insert(Packet p, int pos) {
    if(pos>=getNumOfPacks()) {
        return -1;
    }
    data[pos]=p;
    return 0;
}
