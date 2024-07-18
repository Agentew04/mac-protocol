#include "Frame.h"

// Definições das variáveis estáticas
int Frame::readingBit;
std::vector<bit> Frame::startFrameDelimiter;
std::vector<bit> Frame::payloadLength;
std::vector<bit> Frame::transferAddress;
std::vector<bit> Frame::receiverAddress;
std::vector<bit> Frame::payloadFrameNumber;
std::vector<bit> Frame::ack;
std::vector<bit> Frame::ackNumber;
std::vector<bit> Frame::crc;
std::vector<bit> Frame::parityBit;
std::vector<bit> Frame::padding;
std::vector<bit> Frame::payload;
std::vector<bit> Frame::endFrameDelimiter;

std::vector<Frame> Frame::generateFrames(const std::vector<bit>& data) {
    std::vector<Frame> frames;
    
    readingBit = 0;

    startFrameDelimiter = {1, 1, 0, 0, 1, 0, 1, 0};

    endFrameDelimiter = {1, 1, 1, 1, 1, 1, 1, 0};

    payloadLength = calculatePayloadLength(data);

    transferAddress = calculateTransferAddress(data, &readingBit);

    receiverAddress = calculateReceiverAddress(data, &readingBit);

    payloadFrameNumber = calculatePayloadFrameNumber(data, &readingBit);

    ack = {0};

    ackNumber = {0, 0, 0, 0, 0, 0};

    crc = calculateCrc(data);

    return frames;
}

std::vector<bit> Frame::calculatePayloadLength(const std::vector<bit>& data) {
    std::vector<bit> payloadLength;

    //separar o tamanho do payload em 8 bits
    int size = data.size() - 102;
    for (int i = 0; i < 8; i++) {
        payloadLength.push_back((size >> i) & 1);
    }

    return payloadLength;
}

std::vector<bit> Frame::calculateTransferAddress(const std::vector<bit>& data, int *readingBit) {
    std::vector<bit> transferAddress;

    //retirar os primeiros 48 bits do vetor de dados
    for (int i = *readingBit; i < 48; i++) {
        transferAddress.push_back(data[i]);
    }

    *readingBit = 48;

    return transferAddress;
}

std::vector<bit> Frame::calculateReceiverAddress(const std::vector<bit>& data, int *readingBit) {
    std::vector<bit> receiverAddress;

    //retirar os proximos 48 bits do vetor de dados
    for (int i = *readingBit; i < 96; i++) {
        receiverAddress.push_back(data[i]);
    }

    *readingBit = 96;

    return receiverAddress;
}

std::vector<bit> Frame::calculatePayloadFrameNumber(const std::vector<bit>& data, int *readingBit) {
    std::vector<bit> payloadFrameNumber;

    //retirar os proximos 8 bits do vetor de dados
    for (int i = *readingBit; i < 102; i++) {
        payloadFrameNumber.push_back(data[i]);
    }

    *readingBit = 102;

    return payloadFrameNumber;
}

std::vector<bit> Frame::calculateCrc(const std::vector<bit>& data) {
    std::vector<bit> crc;

    // Convert data to std::vector<char>
    std::vector<char> charData(data.begin(), data.end());

    //calcular o crc dos dados
    uint16_t crc16Calculado = crc16(charData.data(), charData.size());

    //separar o crc em 16 bits
    for (int i = 0; i < 16; i++) {
        crc.push_back((crc16Calculado >> i) & 1);
    }

    return crc;
}

std::vector<bit> Frame::toBits() const {
    std::vector<bit> bits;
    // Implemente aqui a geracao de bits a partir do frame
    return bits;
}
Frame Frame::fromBits(const std::vector<bit>& bits) {
    Frame frame;

    
    // Implemente aqui a geracao de um frame a partir dos bits
    return frame;
}
