#include "Transmitter.h"
#include "Receiver.h"
#include <iostream>

// Construtor para o transmissor que quer enviar dados
Transmitter::Transmitter(std::vector<Frame>& data) {
    for (int i = 0; i < data.size(); ++i) {
        data[i].payloadFrameNumber = i % (windowSize + 1);
    }
    this->data = data;
}

// Verifica se todos os dados foram enviados e todos os ACKs recebidos
bool Transmitter::finished() const {
    return base == data.size() + 1; // Todos os pacotes confirmados
}

// Define o receptor que vai receber os dados
void Transmitter::setReceiver(Receiver* receiver) {
    this->receiver = receiver;

    for (auto& frame : data) {
        frame.receiverAddress = receiver->address
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
    if (frame.isAck) {
        int ackNum = frame.payloadFrameNumber;
        if (ackNum >= base) {
            base = ackNum + 1;
            std::cout << "Recebi ACK para frame " << ackNum << std::endl;
            // Remove os frames confirmados do windowBuffer
            while (!windowBuffer.empty() && windowBuffer.front().seqNum <= ackNum) {
                timeSentMap.erase(windowBuffer.front().seqNum);
                windowBuffer.pop();
            }
        }
    }
}

// Função para atualizar o estado do transmissor
void Transmitter::update() {
    // Envia todos os frames da janela atual
    while (nextSeqNum < base + windowSize && nextSeqNum <= data.size()) {
        Frame frame = data[nextSeqNum - 1];

        if (!channel->shouldDrop()) {
            windowBuffer.push(frame); // Adiciona ao buffer da janela
            frame = channel->pass(frame);
            receiver->receive(frame);
            std::cout << "Enviando frame " << nextSeqNum << std::endl;
            timeSentMap[frame.payloadFrameNumber] = std::chrono::steady_clock::now();

        } else {
            std::cout << "Pacote " << nextSeqNum << " foi dropado" << std::endl;
        }

        nextSeqNum++;
    }

    // Verifica timeouts
    auto now = std::chrono::steady_clock::now();
    for (auto& entry : timeSentMap) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - entry.second).count() > timeoutInterval) {
            // Retransmite o frame
            Frame frame = data[entry.first];
            std::cout << "Timeout! Retransmitindo frame " << entry.first << std::endl;
            receiver->receive(frame);
            timeSentMap[frame.payloadFrameNumber] = std::chrono::steady_clock::now();
        }
    }


    // Se a janela estiver vazia, reinicia o nextSeqNum
    if (base == nextSeqNum) {
        nextSeqNum = base;
    }

    currenttick++;
}
