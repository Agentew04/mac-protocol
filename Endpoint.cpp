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

        std::cout << "Recebi frame " << (int)frame.payloadFrameNumber << std::endl;
    }

    // envia todos da janela atual
    for (int i = nextFrameToSend; i < nextFrameToSend + windowSize && i < data.size(); i++) {
        Frame frame = data[i];

        if(!channel->shouldDrop()){
            frame = channel->pass(frame);
            receiver->receive(frame);
            std::cout << "Enviando frame " << i << std::endl;
        }
    }
    currentTick++;
}
