#include "PacketStream.h"

int PacketStream::initFile(char* filename){
            ifstream f(filename, ios::in|ios::binary|ios::ate);

            if(f.is_open()) {
                size=f.tellg();
                char* image=new char[size];
                f.seekg(0, ios::beg);
                f.read(image, size);
                f.close();
                int tmpSize=(static_cast<int>(size));
                float p=tmpSize/MAX_PACKET_SIZE;
                packetNumber=ceil(p);
                packetNumber=(packetNumber==0)?1:packetNumber;
                data= new Packet[packetNumber];
                int i=0;
                while(i<packetNumber) {
                    if(i==packetNumber-1) {
                        int lastPacketSize=size%(MAX_PACKET_SIZE);
                        char* tmp=new char[lastPacketSize];
                        int j=0;
                        while(j<lastPacketSize) {
                            tmp[j]=image[j];
                            j++;
                        }
                        tmp[j]=0;
                        data[i].setData(tmp);
                        data[i].setSeqAckNum(i, -1);
                        break;
                    }
                    else {
                        int tmp_size = MAX_PACKET_SIZE;
                        char* tmp=new char[tmp_size];
                        int j=0;
                        while(j<(MAX_PACKET_SIZE)) {
                            tmp[j]=image[j];
                            j++;
                        }
                        tmp[j]=0;
                        data[i].setData(tmp);
                        data[i].setSeqAckNum(i, -1);
                        image+=(MAX_PACKET_SIZE);
                    }
                    i++;
                }
                flg=true;
                return 0;
                if (!image)
                    delete image;
            }
            else
                return -1;
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
streampos PacketStream::getFileSize() {
    return size;
}
Packet* PacketStream::get(int x) {
    if(x>=getNumOfPacks())
        return NULL;
    return &data[x];
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
