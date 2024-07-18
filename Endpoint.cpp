#include "Endpoint.h"

#include <iostream>

Endpoint::Endpoint(std::vector<Frame>& data) {
    this->data = data;
}

Endpoint::Endpoint() {
}

bool Endpoint::finished() const {
    return data.empty() && incomingBuffer.empty();
}

void Endpoint::setReceiver(Endpoint* receiver) {
    this->receiver = receiver;


    for(auto& frame: data){
        frame.receiverAddress = receiver->address;
        frame.transmitterAddress = address;
        frame.calculateRedundancy();
        std::cout << frame.checkCrc() << std::endl;
    }
}

void Endpoint::setChannel(Channel* channel) {
    this->channel = channel;
}

void Endpoint::receive(const Frame& frame) {
    incomingBuffer.push(frame);
}



void Endpoint::update() {
    
    while (!incomingBuffer.empty()) {
        Frame frame = incomingBuffer.front();
        incomingBuffer.pop();

        if(!frame.checkCrc()){
            std::cout << "Recebi frame com erro! Dropando" << std::endl;
        }else{
            if (frame.ack) {
                // Recebeu um ACK
                int ackNum = frame.payloadFrameNumber;
                if (ackNum >= base) {
                    base = ackNum + 1;
                    std::cout << "Recebi ACK para frame " << ackNum << std::endl;
                }
            } else {
                // Recebeu um dado, envia ACK
                Frame ackFrame = Frame::cre
                ackFrame.payloadFrameNumber = frame.payloadFrameNumber;
                ackFrame.ack = true;
                receiver->receive(ackFrame);
                std::cout << "Enviando ACK para frame " << (int)frame.payloadFrameNumber << std::endl;
            }

        }
    }

    // envia todos da janela atual
    //for alguma coisa
        Frame frame = data[i];

        if(!channel->shouldDrop()){
            frame = channel->pass(frame);
            receiver->receive(frame);
            std::cout << "Enviando frame " << i << std::endl;
        }else{
            std::cout << "Pacote " << i << " foi dropado" << std::endl;
        }
    }

    currentTick++;
}


