#include "Endpoint.h"

#include <iostream>

Endpoint::Endpoint(std::vector<Frame>& data) {
    this->data = data;
}

Endpoint::Endpoint() {
}

bool Endpoint::finished() const {
    return (uint32_t)base == data.size() + 1;
}

void Endpoint::setReceiver(Endpoint* receiver) {
    this->receiver = receiver;


    for(auto& frame: data){
        frame.receiverAddress = receiver->address;
        frame.transmitterAddress = address;
        frame.calculateRedundancy();
    }
}

void Endpoint::setChannel(Channel* channel) {
    this->channel = channel;
}

void Endpoint::receive(const Frame& frame) {
    incomingBuffer.push(frame);
}



void Endpoint::update() {
    std::cout << "Incoming size: " << incomingBuffer.size() << std::endl;
    while (!incomingBuffer.empty()) {
        Frame frame = incomingBuffer.front();
        incomingBuffer.pop();

        if(!frame.checkRedundancy()){
            std::cout << "Recebi frame com erro! Dropando" << std::endl;
            continue;
        }
        if (frame.ack) {
            // eh um ACK
            std::cout << "Recebi ACK para frame " << (int)frame.ackNumber << std::endl;
        } else {
            // eh um dado
            if(frame.payloadFrameNumber != lastReceivedFrameNumber+1){
                std::cout << "Recebi frame fora de ordem! Dropando" << std::endl;
                continue;
            }
            lastReceivedFrameNumber = frame.payloadFrameNumber;

            Frame ackFrame = Frame::generateAck(frame.payloadFrameNumber);
            ackFrame.receiverAddress = frame.transmitterAddress;
            ackFrame.transmitterAddress = frame.receiverAddress;
            ackFrame.calculateRedundancy();
            if(!channel->shouldDrop()){
                std::cout << "Enviando ACK para frame " << (int)frame.payloadFrameNumber << std::endl;
                ackFrame = channel->pass(ackFrame);
                receiver->receive(ackFrame);
            }else{
                std::cout << "ACK para frame" << (int)frame.payloadFrameNumber << " foi dropado" << std::endl;
            }
        }
    }

    while (nextSeqNum < base + windowSize && (uint32_t)nextSeqNum <= data.size()) {
        Frame frame = data[nextSeqNum - 1];

        windowBuffer.push(frame); // Adiciona ao buffer da janela
        if (!channel->shouldDrop()) {
            frame = channel->pass(frame);
            receiver->receive(frame);
            std::cout << "Enviando frame " << (int)frame.payloadFrameNumber << std::endl;
        } else {
            std::cout << "Pacote " << nextSeqNum << " foi dropado" << std::endl;
        }

        nextSeqNum++;
    }

    // Se a janela estiver vazia, reinicia o nextSeqNum
    if (base == nextSeqNum) {
        nextSeqNum = base;
    }

    currentTick++;
}


