#include "Frame.h"
#include "errordetection.h"

#include <iostream>
#include <cmath>
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

const size_t headerSize = 8+8+(48*2)+6+1+6+16+1+2;

std::vector<Frame> Frame::generateFrames(const std::vector<bit>& data){
    std::vector<Frame> frames;

    const int maxPayloadSize = 255*4.0;
    int frameCount = data.size() / maxPayloadSize;
    frameCount += data.size() % maxPayloadSize == 0 ? 0 : 1;

    std::cout << "Generating " << frameCount << " frames..." << std::endl;

    int dataPointer = 0;
    for(int i=0; i<frameCount; i++){
        Frame frame;
        frame.startFrameDelimiter = 0xCA;

        int payloadSize = std::max(0, std::min(maxPayloadSize, (int)data.size() - dataPointer));
        int paddingSize = 4 - (payloadSize % 4);
        if(paddingSize == 4) paddingSize = 0;

        if(paddingSize > 3 || paddingSize < 0 || paddingSize+payloadSize > 255*4 || (payloadSize+paddingSize) % 4 != 0){
            std::cout << "[Frame::generateFrames] Erro no padding! Tamanho do payload: " << payloadSize << "; Tamanho do padding: " << paddingSize << std::endl;
        }
        frame.payloadLength = (int)std::ceil(payloadSize/4.0f);
        frame.paddingSize = paddingSize;

        frame.receiverAddress = 0;
        frame.transmitterAddress = 0;

        frame.payloadFrameNumber = i;
        frame.ack = 0;
        frame.ackNumber = 0;

        frame.crc = 0; // nao calculo agora por causa dos enderecos
        frame.parityBit = 0;


        for(int j=0; j<payloadSize; j++){
            frame.payload.push_back(data[dataPointer]);
            dataPointer++;
        }
        if((size_t)(frame.payloadLength*4-frame.paddingSize) != frame.payload.size()){
            std::cout << "[Frame::generateFrames] Erro no tamanho do payload! Expected " << frame.payloadLength*4-frame.paddingSize << ", got " << frame.payload.size() << "." << std::endl;
        }
        
        for(int j=0; j<paddingSize; j++){
            frame.payload.push_back(0);
        }

        if(frame.payload.size() != frame.payloadLength*4){
            std::cout << "[Frame::generateFrames] Erro no tamanho do payload! Expected " << frame.payloadLength*4 << ", got " << frame.payload.size() << "." << std::endl;
        }

        if(frame.payload.size() % 4 != 0){
            std::cout << "[Frame::generateFrames] Erro no padding! Tamanho do payload: " << frame.payload.size() << std::endl;
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

std::vector<bit> intToBits(uint64_t num, size_t bits){
    std::vector<bit> bitsVector;
    bitsVector.reserve(bits);
    for(size_t i = 0; i < bits; i++){
        bitsVector.push_back((num >> i) & 1);
    }
    return bitsVector;
}

uint64_t bitsToInt(const std::vector<bit>& bits, uint32_t start, size_t length){
    uint64_t num = 0;
    for(size_t i = 0; i < length; i++){
        num |= bits[start + i] << i;
    }
    return num;
}

std::vector<bit> Frame::toBits() const{
    std::vector<bit> bits;
    bits.reserve(headerSize + payload.size() + 8);
    
    auto startFrameDelimiterBits = intToBits(startFrameDelimiter, 8);
    bits.insert(bits.end(), startFrameDelimiterBits.begin(), startFrameDelimiterBits.end());

    auto payloadLengthBits = intToBits(payloadLength, 8);
    bits.insert(bits.end(), payloadLengthBits.begin(), payloadLengthBits.end());

    auto transmitterBits = intToBits(transmitterAddress, 48);
    bits.insert(bits.end(), transmitterBits.begin(), transmitterBits.end());

    auto receiverBits = intToBits(receiverAddress, 48);
    bits.insert(bits.end(), receiverBits.begin(), receiverBits.end());

    auto payloadFrameNumberBits = intToBits(payloadFrameNumber, 6);
    bits.insert(bits.end(), payloadFrameNumberBits.begin(), payloadFrameNumberBits.end());

    bits.push_back(ack);

    auto ackNumberBits = intToBits(ackNumber, 6);
    bits.insert(bits.end(), ackNumberBits.begin(), ackNumberBits.end());

    auto crcBits = intToBits(crc, 16);
    bits.insert(bits.end(), crcBits.begin(), crcBits.end());

    bits.push_back(parityBit);

    auto paddingSizeBits = intToBits(paddingSize, 2);
    bits.insert(bits.end(), paddingSizeBits.begin(), paddingSizeBits.end());

    if(bits.size() != headerSize){
        std::cout << "[Frame::toBits] Erro no tamanho do header! Expected " << headerSize << ", got " << bits.size() << "." << std::endl;
    }

    if(payload.size() % 4 != 0){
        std::cout << "[Frame::toBits] Erro no tamanho do payload! Expected multiple of 4, got " << payload.size() << "." << std::endl;
    }

    if(payloadLength*4>1020){
        std::cout << "[Frame::toBits] Tamanho do payload maior que 1020! Expected max 1020, got " << payloadLength*4 + paddingSize << "." << std::endl;
    }
    if(payload.size() != payloadLength*4){
        std::cout << "[Frame::toBits] Tamanho do payload nao corresponde! Expected " << payloadLength*4 + paddingSize << ", got " << payload.size() << "." << std::endl;
    }

    bits.insert(bits.end(), payload.begin(), payload.end());

    auto endFrameDelimiterBits = intToBits(endFrameDelimiter, 8);
    bits.insert(bits.end(), endFrameDelimiterBits.begin(), endFrameDelimiterBits.end());

    if(bits.size() != headerSize + payloadLength*4 + 8){
        std::cout << "[Frame::toBits] Erro no tamanho do frame! Expected " << headerSize + payloadLength*4 + paddingSize + 8 << ", got " << bits.size() << "." << std::endl;
    }

    return bits;
}

Frame Frame::fromBits(const std::vector<bit>& bits){
    Frame f;

    f.startFrameDelimiter = bitsToInt(bits, 0, 8);
    f.payloadLength = bitsToInt(bits, 8, 8);
    f.transmitterAddress = bitsToInt(bits, 16, 48);
    f.receiverAddress = bitsToInt(bits, 64, 48);
    f.payloadFrameNumber = bitsToInt(bits, 112, 6);
    f.ack = bits[118];
    f.ackNumber = bitsToInt(bits, 119, 6);
    f.crc = bitsToInt(bits, 125, 16);
    f.parityBit = bits[141];
    f.paddingSize = bitsToInt(bits, 142, 2);

    if(bits.size() != headerSize + f.payloadLength*4 + 8){
        std::cout << "[Frame::fromBits] Erro no tamanho do frame! Expected " << headerSize + f.payloadLength*4 + f.paddingSize + 8 << ", got " << bits.size() << "." << std::endl;
    }

    int payloadStart = 144;
    int payloadEnd = payloadStart + f.payloadLength*4;
    f.payload = std::vector<bit>(bits.begin() + payloadStart, bits.begin() + payloadEnd);

    f.endFrameDelimiter = bitsToInt(bits, payloadEnd, 8);
    return f;
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
    std::vector<bit> dataBits = payload;
    crc = crc16((char*)payload.data(), payload.size());

    std::vector<bit> frameBits = toBits();
    std::vector<bit> headerBits(frameBits.begin(), frameBits.begin() + headerSize);
    // remover bit de paridade da conta
    bit pad1 = headerBits.back(); 
    headerBits.pop_back();
    bit pad2 = headerBits.back(); 
    headerBits.pop_back();
    headerBits.pop_back(); // remove o bit de paridade
    headerBits.push_back(pad2);
    headerBits.push_back(pad1);

    parityBit = parity((char*)headerBits.data(), headerBits.size());
}

bool Frame::checkRedundancy(){
    Frame f2 = Frame(*this);

    f2.calculateRedundancy();

    return crc == f2.crc && parityBit == f2.parityBit;
}
