#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <thread>

#include "Endpoint.h"
#include "Frame.h"
#include "errordetection.h"

typedef unsigned char bit;

std::vector<bit> generateData(){
    std::mt19937 gen(time(nullptr));

    // numero de bits
    std::uniform_int_distribution<int> dist(30'000, 50'000);
    // bit aleatorio
    std::uniform_int_distribution<int> dist2(0, 1);

    std::vector<bit> data;
    int bits = dist(gen);
    data.reserve(bits);

    for(int i = 0; i < bits; i++){
        data.push_back(dist2(gen));
    }
    return data;
}

void printData(const std::vector<bit> &data){
    for(auto &d : data){
        std::cout << (int)d;
    }
    std::cout << std::endl;
}

bool selftest(){
    std::cout << "Self testing..." << std::endl;
    // testar tobit e frombit
    std::vector<bit> data = generateData();
    auto frames = Frame::generateFrames(data);
    std::cout << "\t[OK] Frame::generateFrames" << std::endl;
    for(auto &f : frames){
        auto bits = f.toBits();
        auto f2 = Frame::fromBits(bits);
        if(f.startFrameDelimiter != f2.startFrameDelimiter){
            std::cout << "startFrameDelimiter diferente!" << std::endl;
            return false;
        }
        if(f.payloadLength != f2.payloadLength){
            std::cout << "payloadLength diferente!" << std::endl;
            return false;
        }
        if(f.transmitterAddress != f2.transmitterAddress){
            std::cout << "transmitterAddress diferente!" << std::endl;
            return false;
        }
        if(f.receiverAddress != f2.receiverAddress){
            std::cout << "receiverAddress diferente!" << std::endl;
            return false;
        }
        if(f.payloadFrameNumber != f2.payloadFrameNumber){
            std::cout << "payloadFrameNumber diferente!" << std::endl;
            return false;
        }
        if(f.ack != f2.ack){
            std::cout << "ack diferente!" << std::endl;
            return false;
        }
        if(f.ackNumber != f2.ackNumber){
            std::cout << "ackNumber diferente!" << std::endl;
            return false;
        }
        if(f.crc != f2.crc){
            std::cout << "crc diferente!" << std::endl;
            return false;
        }
        if(f.parityBit != f2.parityBit){
            std::cout << "parityBit diferente!" << std::endl;
            return false;
        }
        if(f.paddingSize != f2.paddingSize){
            std::cout << "paddingSize diferente! F1=" << (int)f.paddingSize << "; F2=" << (int)f2.paddingSize << ";" << std::endl;
            return false;
        }
        if(f.endFrameDelimiter != f2.endFrameDelimiter){
            std::cout << "endFrameDelimiter diferente! F1=" << (int)f.endFrameDelimiter << "; F2=" << (int)f2.endFrameDelimiter << std::endl;
            return false;
        }
        for(size_t i = 0; i < f.payload.size(); i++){
            if(f.payload[i] != f2.payload[i]){
                std::cout << "payload diferente em i=" << i << "." << std::endl;
                return false;
            }
        }
    }

    std::cout << "\t[OK] Frame::toBits and Frame::fromBits" << std::endl;

    // testar crc16
    for(auto& frame: frames){
        frame.calculateRedundancy();
        if(!frame.checkRedundancy()){
            std::cout << "Erro no crc16!" << std::endl;
            return false;
        }

        auto bits = frame.toBits();
        bits[0] = !bits[0];
        auto frame2 = Frame::fromBits(bits);
        if(frame2.checkRedundancy()){
            std::cout << "Erro de redundancia!" << std::endl;
            return false;
        }
    }
    std::cout << "\t[OK] Frame::calculateRedundancy and Frame::checkRedundancy" << std::endl;

    return true;
}

int main(void){
    if(!selftest()){
        std::cout << "Self test failed!" << std::endl;
        return 1;
    }
    
    Channel enlace;

    // gerar os dados
    std::cout << "Generating data... ";
    std::vector<bit> data = generateData();
    std::cout << "Done (" << data.size() << " bits)!" << std::endl;

    // dividir os bits de dados nos frames(e montar os headers)
    auto frames = Frame::generateFrames(data);

    // simular a troca de dados na rede
    Endpoint transmissor = Endpoint(frames);
    transmissor.address = 0x01'02'03'04'05'06;
    Endpoint receptor = Endpoint();
    receptor.address = 0x06'05'04'03'02'01;

    transmissor.setReceiver(&receptor);
    receptor.setReceiver(&transmissor);
    transmissor.setChannel(&enlace);
    receptor.setChannel(&enlace);

    int i = 0;
    while(!transmissor.finished() && i<10) {
        // a cada iteracao, atualiza um tick em
        // cada maquina
        std::cout << "Tick " << i << std::endl;
        if(i % 2 == 0){
            transmissor.update();
        } else {
            receptor.update();
        }
        i++;
    }

    //printData(data);
    return 0;
}