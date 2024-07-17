#ifndef __FRAME_H__
#define __FRAME_H__

#include <vector>

typedef unsigned char bit;

class Frame {
public:

    /// @brief Gera frames a partir de dados
    /// @param data Vetor de bits com os dados
    /// @return Vetor de frames
    static std::vector<Frame> generateFrames(const std::vector<bit>& data);

    /// @brief Retorna um array de bits que representa a estrutura do frame
    /// conforme a tabela do nosso protocolo.
    std::vector<bit> toBits() const;

    /// @brief Reconstroi um frame a partir de um array de bits
    /// @param bits Array de bits que representa um frame
    /// @return Frame reconstituido
    static Frame fromBits(const std::vector<bit>& bits);

    // preencher as variaveis da tabela aqui!!
};

#endif