#ifndef __FRAME_H__
#define __FRAME_H__

#include <vector>
#include <stdint.h>

typedef unsigned char bit;

class Frame {
public:

    /// @brief Gera frames a partir de dados
    /// @param data Vetor de bits com os dados
    /// @return Vetor de frames
    static std::vector<Frame> generateFrames(const std::vector<bit>& data);


    /// @brief Gera um ack a partir de um frame
    /// @param int ackNumber Numero do ack
    /// @return Frame com o ack
    static Frame generateAck(int ackNumber);


    /// @brief Retorna um array de bits que representa a estrutura do frame
    /// conforme a tabela do nosso protocolo.
    std::vector<bit> toBits() const;

    /// @brief Reconstroi um frame a partir de um array de bits
    /// @param bits Array de bits que representa um frame
    /// @return Frame reconstituido
    static Frame fromBits(const std::vector<bit>& bits);

    /// @brief Construtor padrao
    Frame();

    /// @brief Imprime o frame na tela
    void print()const;

    void calculateRedundancy();

    bool checkRedundancy() const;

    

    /*Start Frame Delimiter	8 bits	0xCA	Define o início do quadro
    Payload Length	8 bits	0-255	Tamanho do payload em bits dividido por 4
    Endereço Transmissor	48 bits	-	Endereço MAC do transmissor
    Endereço Receptor	48 bits	-	Endereço MAC do destinatário
    Payload Frame Number	6 bits	0-63	Número do quadro
    ACK	1 bit	0 ou 1	Indica se o próximo campo define o ultimo quadro recebido
    ACK Number	6 bits	0-63	Número do quadro recebido ou nada de ACK for 0
    CRC	16 bits	-	Código de detecção de erro no payload
    Parity Bit	1 bit	0 ou 1	Bit de paridade para detecção de erro no header. Inclui o CRC no cálculo
    Padding Size	2 bits	0-3	Tamanho do padding a ser adicionado ao payload
    Payload	0-255 bits	-	Dados a serem transmitidos. Deve ser um múltiplo de 4. Se não for recebe um padding para completar de acordo com PaddingSize
    End Frame Delimiter*/

    // preencher as variaveis da tabela aqui!!
    uint8_t startFrameDelimiter; // 8 bits
    uint8_t payloadLength; // 8 bits
    uint64_t transmitterAddress; // 48 bits, but stored in a 64-bit integer for simplicity
    uint64_t receiverAddress; // 48 bits, similar to transmitterAddress
    uint8_t payloadFrameNumber; // 6 bits, but stored in an 8-bit integer
    uint8_t ack; // 1 bit, but stored in an 8-bit integer
    uint8_t ackNumber; // 6 bits, stored in an 8-bit integer
    uint16_t crc; // 16 bits
    uint8_t parityBit; // 1 bit, stored in an 8-bit integer
    uint8_t paddingSize; // 2 bits, stored in an 8-bit integer
    std::vector<uint8_t> payload; // Variable length, each element represents 8 bits
    uint8_t endFrameDelimiter; // 8 bits, assuming it matches the start frame delimiter
};


#endif