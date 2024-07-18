#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <random>

#include "Frame.h"

/// @brief Representa um canal ruidoso
class Channel {
public:
    /// @brief Cria um canal
    Channel();

    /// @brief Retorna se o frame deve ser dropado
    /// @return true se deve ser dropado, false caso contrario
    bool shouldDrop();

    /// @brief Passa um frame pelo canal e modifica alguns bits as vezes!
    /// @param frame O frame a ser passado
    /// @return O frame modificado
    Frame pass(Frame frame);

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<double> dropDistribution;
    std::uniform_real_distribution<double> bitDistribution;
    double dropThreshold = 1/100.0; // 1% de chance de dropar
    double bitErrorRate = 0.1/100.0; // 0,1% de chance de erro de bit
};

#endif