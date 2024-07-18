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
}

void Endpoint::setChannel(Channel* channel) {
    this->channel = channel;
}

void Endpoint::receive(const Frame& frame) {
    incomingBuffer.push(frame);
}

void Endpoint::update() {
    if (!incomingBuffer.empty()) {
        
    }
}
