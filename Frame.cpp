#include "Frame.h"
#include "errordetection.h"

#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

/*
seguindo o readme:

header:
- start frame delimiter (1 byte) = 0-7
- payload length (1 byte) = 8-15
- transmitter address (48 bits) = 16-63
- receiver address (48 bits) = 64-111
- payload frame number (6 bits) = 112-117
- ack (1 bit) = 118
- ack number (6 bits) = 119-124
- crc (16 bits) = 125-140
- parity bit (1 bit) = 141
- padding size (2 bits) = 142-143
- payload (0-1020 bits) = 144-x (multiplos de 4)
- end frame delimiter (1 byte) = x+1-x+8
*/

std::vector<Frame> Frame::generateFrames(const std::vector<bit>& data){
    std::vector<Frame> frames;

    int maxPayloadSize = 255*4.0;
    int frameCount = data.size() / maxPayloadSize;
    frameCount += data.size() % maxPayloadSize == 0 ? 0 : 1;

    std::cout << "Generating " << frameCount << " frames..." << std::endl;

    int dataPointer = 0;
    for(int i=0; i<frameCount; i++){
        Frame frame;
        frame.startFrameDelimiter = 0xCA;

        int payloadSize = std::max(0, std::min(maxPayloadSize, (int)data.size() - dataPointer));
        frame.payloadLength = payloadSize / 4;

        frame.receiverAddress = 0;
        frame.transmitterAddress = 0;

        frame.payloadFrameNumber = i;
        frame.ack = 0;
        frame.ackNumber = 0;

        frame.crc = 0; // nao calculo agora por causa dos enderecos
        frame.parityBit = 0;

        int paddingSize = 4 - (payloadSize % 4);
        frame.paddingSize = paddingSize;

        for(int j=0; j<payloadSize; j++){
            frame.payload.push_back(data[dataPointer]);
            dataPointer++;
        }
        for(int j=0; j<paddingSize; j++){
            frame.payload.push_back(0);
        }

        if(frame.payload.size() % 4 != 0){
            std::cout << "Erro no padding!" << std::endl;
        }
        
        frame.endFrameDelimiter = 0xFE;
        frames.push_back(frame);
    }
    return frames;
}

Frame Frame::generateAck(int ackNumber){
    std::vector<Frame> acks;
    Frame ack;
    ack.startFrameDelimiter = 0xCA;
    ack.payloadLength = 0;
    ack.transmitterAddress = 0;
    ack.receiverAddress = 0;
    ack.payloadFrameNumber = 0;
    ack.ack = 1;
    ack.ackNumber = ackNumber;
    ack.crc = 0;
    ack.parityBit = 0;
    ack.paddingSize = 0;
    ack.endFrameDelimiter = 0xCA;

    return ack;
}

std::vector<bit> Frame::toBits() const{
    std::vector<bit> bits;
    bits.reserve(8 + 8 + 48 + 48 + 6 + 1 + 6 + 16 + 1 + 2 + 8);
    
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

void Frame::calculateRedundancy(){
    std::vector<bit> headerBits = {startFrameDelimiter, payloadLength, (bit)(transmitterAddress, receiverAddress), payloadFrameNumber, ack, ackNumber, parityBit, paddingSize};
    std::vector<bit> dataBits = payload;

    // calcular o CRC16
    crc = crc16((char*)payload.data(), payload.size());

    // calcular o bit de paridade
    parityBit = parity((char*)headerBits.data(), headerBits.size());
}

bool Frame::checkCrc(){
    return crc == crc16((char*)payload.data(), payload.size());
}