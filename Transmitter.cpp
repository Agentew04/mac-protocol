#include "Transmitter.h"
#include "Receiver.h"
#include <iostream>

// Construtor para o transmissor que quer enviar dados
Transmitter::Transmitter(std::vector<Frame>& data) {

    /*// change payload number to a cyclic
    for (size_t i = 0; i < data.size(); i++) {
        data[i].payloadFrameNumber = i % (windowSize+1);
    }*/

    this->data = data;
}

// Verifica se todos os dados foram enviados e todos os ACKs recebidos
bool Transmitter::finished() const {
    return (size_t)base >= data.size(); // Todos os pacotes confirmados
}

// Define o receptor que vai receber os dados
void Transmitter::setReceiver(Receiver* receiver) {
    this->receiver = receiver;

    for (auto& frame : data) {
        frame.receiverAddress = receiver->address;
        frame.transmitterAddress = address;
        frame.calculateRedundancy();
    }
}

// Define o canal que o transmissor está localizado
void Transmitter::setChannel(Channel* channel) {
    this->channel = channel;
}

// Recebe um ACK do receptor e atualiza a janela
void Transmitter::receiveAck(const Frame& frame) {
    if(!frame.ack){
        std::cout << "[Transmitter] Recebi um frame que não é um ACK" << std::endl;
        return;
    }

    int ackNum = (int)frame.ackNumber;
    std::cout << "[Transmitter] Recebi ACK para frame " << ackNum << std::endl;
    if (ackNum >= base) {
        base = ackNum + 1;
        std::cout << "[Transmitter] Eh um novo ACK. Nova base: " << base << std::endl;
        // Remove os frames confirmados do windowBuffer
        while (!windowBuffer.empty() && windowBuffer.front().payloadFrameNumber < base) {
            std::cout << "[Transmitter] Removendo frame " << (int)windowBuffer.front().payloadFrameNumber << " da janela" << std::endl;
            windowBuffer.pop();
            timeSentMap.erase(ackNum);
        }
    }else{
        // cliente nao recebeu algum pacote, retransmite a partir desse ack
        nextSeqNum = ackNum + 1;
        std::cout << "[Transmitter] Cliente nao recebeu pacote, retransmitindo a partir de " << nextSeqNum << std::endl;
    }
}

// Função para atualizar o estado do transmissor
void Transmitter::update() {
    std::cout << "[Transmitter] Itens na janela atual: " << windowBuffer.size() << std::endl;

    // Envia todos os frames da janela atual
    while (nextSeqNum < base + windowSize && (size_t)nextSeqNum <= data.size()) {
        Frame frame = data[nextSeqNum - 1];

        if (!channel->shouldDrop()) {
            windowBuffer.push(frame); // Adiciona ao buffer da janela
            frame = channel->pass(frame);
            receiver->receiveFrame(frame);
            std::cout << "Enviando frame " << nextSeqNum << std::endl;
            timeSentMap[frame.payloadFrameNumber] = std::chrono::steady_clock::now();

        } else {
            std::cout << "Pacote " << nextSeqNum << " foi dropado" << std::endl;
        }
        nextSeqNum++;
    }

    auto now = std::chrono::steady_clock::now();
    for (auto& entry : timeSentMap) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - entry.second).count() > timeoutInterval) {
            // Retransmite o frame
            Frame frame = data[entry.first];
            std::cout << "Timeout! Retransmitindo frame " << entry.first << std::endl;
            receiver->receiveFrame(frame);
            timeSentMap[frame.payloadFrameNumber] = std::chrono::steady_clock::now();
        }
    }


    // Se a janela estiver vazia, reinicia o nextSeqNum
    if (base == nextSeqNum) {
        nextSeqNum = base;
    }

    currenttick++;
}
