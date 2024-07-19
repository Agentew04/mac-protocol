#include "Receiver.h"

#include "Transmitter.h"

#include <iostream>

Receiver::Receiver() {
}

void Receiver::setTransmitter(Transmitter* transmitter) {
    this->transmitter = transmitter;
}

void Receiver::setChannel(Channel* channel) {
    this->channel = channel;
}

void Receiver::receiveFrame(const Frame& frame) {

    if(!frame.checkRedundancy()){
        std::cout << "[Receiver] Recebi frame com erro! Dropando" << std::endl;
        return;
    }

    if (frame.ack) {
        std::cout << "[Receiver] Recebi um ACK, nao sei oq fazer" << std::endl;
        return; // ignoramos ACKS
    }

    if (frame.payloadFrameNumber == awaitingNum) {
        // recebi o frame certo
        Frame ackFrame = Frame::generateAck(frame.payloadFrameNumber);
        ackFrame.receiverAddress = frame.transmitterAddress;
        ackFrame.transmitterAddress = frame.receiverAddress;
        ackFrame.calculateRedundancy();
        awaitingNum++;
        if(!channel->shouldDrop()){
            ackFrame = channel->pass(ackFrame);
            transmitter->receiveAck(ackFrame);
        }

    }else{
        // recebi frame fora de ordem, respondo com ultimo ACK
        std::cout << "[Receiver] Recebi frame fora de ordem! Respondendo com ultimo ACK" << std::endl;
        Frame ackFrame = Frame::generateAck(awaitingNum-1);
        ackFrame.receiverAddress = frame.transmitterAddress;
        ackFrame.transmitterAddress = frame.receiverAddress;
        ackFrame.calculateRedundancy();

        if(!channel->shouldDrop()){
            ackFrame = channel->pass(ackFrame);
            transmitter->receiveAck(ackFrame);
        }else{
            std::cout << "[Receiver] ACK dropado na rede" << std::endl;
        }
    }
    
}

void Receiver::update(){

}


