#include "Frame.h"
#include "errordetection.h"

#include <iostream>
#include <vector>
#include <bitset>



using namespace std;



std::vector<Frame> Frame::generateFrames(const std::vector<bit>& data){
    std::vector<Frame> frames;
    Frame frame;
    int i = 0;
    
    while(i < data.size()){
        frame.startFrameDelimiter = 0xCA;
        frame.payloadLength = 0;
        frame.transmitterAddress = 0;
        frame.receiverAddress = 0;
        frame.payloadFrameNumber = 0;
        frame.ack = 0;
        frame.ackNumber = 0;
        frame.crc = 0;
        frame.parityBit = 0;
        frame.paddingSize = 0;
        frame.payload.clear();
        frame.endFrameDelimiter = 0xCA;
        
        // preencher o frame com os dados
        // do vetor de bits
        for(int j = 0; j < 8; j++){
            if(i < data.size()){
                frame.payload.push_back(data[i]);
                i++;
            } else {
                frame.payload.push_back(0);
            }
        }
        
        // calcular o CRC16
        frame.crc = crc16((char*)frame.payload.data(), frame.payload.size());
        
        // calcular o bit de paridade
        frame.parityBit = parity((char*)frame.payload.data(), frame.payload.size());
        
        // calcular o padding
        frame.paddingSize = 4 - (frame.payload.size() % 4);
        
        // adicionar o padding
        for(int k = 0; k < frame.paddingSize; k++){
            frame.payload.push_back(0);
        }
        
        // calcular o tamanho do payload
        frame.payloadLength = frame.payload.size() / 4;
        
        // adicionar o frame ao vetor de frames
        frames.push_back(frame);
    }

    return frames;

}

std::vector<bit> Frame::toBits() const{
    std::vector<bit> bits;
    bits.reserve(8 + 8 + 48 + 48 + 6 + 1 + 6 + 16 + 1 + 2 + payload.size() * 8 + 8);
    
    bits.push_back(startFrameDelimiter);
    bits.push_back(payloadLength);
    std::bitset<6> transmitterAddressBits(transmitterAddress);
    bits.push_back(transmitterAddress);
    bits.push_back(receiverAddress);
    bits.push_back(payloadFrameNumber);
    bits.push_back(ack);
    bits.push_back(ackNumber);
    bits.push_back(crc);
    bits.push_back(parityBit);
    bits.push_back(paddingSize);

    for(auto &b : payload){
        bits.push_back(b);
    }

    bits.push_back(endFrameDelimiter);

    return bits;
}

Frame Frame::fromBits(const std::vector<bit>& bits){
    Frame frame;
    int i = 0;
    
    frame.startFrameDelimiter = bits[i];
    i++;
    frame.payloadLength = bits[i];
    i++;

    for(int j = 0; j < 6; j++){
        frame.transmitterAddress |= bits[i] << j;
        i++;
    }

    for(int k = 0; k < 6; k++){
        frame.receiverAddress |= bits[i] << k;
        i++;
    }

    for(int l = 0; l < 6; l++){
        frame.payloadFrameNumber |= bits[i] << l;
        i++;
    }

    frame.ack = bits[i];

    for(int m = 0; m < 6; m++){
        frame.ackNumber |= bits[i] << m;
        i++;
    }

    for(int n = 0; n < 16; n++){
        frame.crc |= bits[i] << n;
        i++;
    }

    frame.parityBit = bits[i];

    for(int o = 0; o < 2; o++){
        frame.paddingSize |= bits[i] << o;
        i++;
    }

    while(i < bits.size() - 8){
        bit byte = 0;
        for(int p = 0; p < 8; p++){
            byte |= bits[i] << p;
            i++;
        }
        frame.payload.push_back(byte);
    }

    frame.endFrameDelimiter = bits[i];

    return frame;

}

Frame::Frame(){
    startFrameDelimiter = 0;
    payloadLength = 0;
    transmitterAddress = 0;
    receiverAddress = 0;
    payloadFrameNumber = 0;
    ack = 0;
    ackNumber = 0;
    crc = 0;
    parityBit = 0;
    paddingSize = 0;
    endFrameDelimiter = 0;
}


void Frame::print()const{
    std::cout << "Start Frame Delimiter: " << (int)startFrameDelimiter << std::endl;
    std::cout << "Payload Length: " << (int)payloadLength << std::endl;
    std::cout << "Transmitter Address: " << transmitterAddress << std::endl;
    std::cout << "Receiver Address: " << receiverAddress << std::endl;
    std::cout << "Payload Frame Number: " << (int)payloadFrameNumber << std::endl;
    std::cout << "ACK: " << (int)ack << std::endl;
    std::cout << "ACK Number: " << (int)ackNumber << std::endl;
    std::cout << "CRC: " << crc << std::endl;
    std::cout << "Parity Bit: " << (int)parityBit << std::endl;
    std::cout << "Padding Size: " << (int)paddingSize << std::endl;
    std::cout << "Payload: ";
    for(auto &p : payload){
        std::cout << (int)p << " ";
    }
    std::cout << std::endl;
    std::cout << "End Frame Delimiter: " << (int)endFrameDelimiter << std::endl;
}
