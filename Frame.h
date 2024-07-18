#ifndef __FRAME_H__
#define __FRAME_H__

#include <vector>
#include <iostream>

#include "errordetection.h"

typedef unsigned char bit;

class Frame {
public:

    /// @brief Gera frames a partir de dados
    /// @param data Vetor de bits com os dados
    /// @return Vetor de frames
    static std::vector<Frame> generateFrames(const std::vector<bit>& data);

    /// @brief Calcula o tamanho do payload
    /// @param data Vetor de bits com os dados
    /// @return Vetor de bits com o tamanho do payload
    static std::vector<bit> calculatePayloadLength(const std::vector<bit>& data);

    /// @brief Calcula o endereco de transferencia
    /// @param data Vetor de bits com os dados
    /// @param readingBit Bit de leitura
    /// @return Vetor de bits com o endereco de transferencia
    static std::vector<bit> calculateTransferAddress(const std::vector<bit>& data, int *readingBit);

    /// @brief Calcula o endereco do receptor
    /// @param data Vetor de bits com os dados
    /// @param readingBit Bit de leitura
    /// @return Vetor de bits com o endereco do receptor
    static std::vector<bit> calculateReceiverAddress(const std::vector<bit>& data, int *readingBit);
    
    
    /// @brief Calcula o numero do frame
    /// @param data Vetor de bits com os dados
    /// @param readingBit Bit de leitura
    /// @return Vetor de bits com o numero do frame
    static std::vector<bit> calculatePayloadFrameNumber(const std::vector<bit>& data, int *readingBit);

    /// @brief Calcula o CRC
    /// @param data Vetor de bits com os dados
    /// @return Vetor de bits com o CRC
    static std::vector<bit> calculateCrc(const std::vector<bit>& data);


    /// @brief Retorna um array de bits que representa a estrutura do frame
    /// conforme a tabela do nosso protocolo.
    std::vector<bit> toBits() const;

    /// @brief Reconstroi um frame a partir de um array de bits
    /// @param bits Array de bits que representa um frame
    /// @return Frame reconstituido
    static Frame fromBits(const std::vector<bit>& bits);

    // preencher as variaveis da tabela aqui!!

private:
    static int readingBit;
    static std::vector<bit> startFrameDelimiter;
    static std::vector<bit> payloadLength;
    static std::vector<bit> transferAddress;
    static std::vector<bit> receiverAddress;
    static std::vector<bit> payloadFrameNumber;
    static std::vector<bit> ack;
    static std::vector<bit> ackNumber;
    static std::vector<bit> crc;
    static std::vector<bit> parityBit;
    static std::vector<bit> padding;
    static std::vector<bit> payload;
    static std::vector<bit> endFrameDelimiter;
};

#endif