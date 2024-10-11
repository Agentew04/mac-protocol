#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#include "Frame.h"
#include "Channel.h"
#include <queue>
#include <unordered_map>
#include <chrono>
#include <vector>


class Receiver;

/// @brief Classe que representa uma ponta do enlace.
/// Transmite dados, espera ACKs. Recebe dados e envia ACKs.
class Transmitter {
public:

    /// @brief Cria um endpoint que quer enviar dados.
    /// @param data Os quadros que vao ser enviados
    Transmitter(std::vector<Frame>& data);


    /// @brief Se o endpoint enviou tudo que precisava e
    /// recebeu todos as confirmações.
    /// @return true se acabou ou false se ainda nao
    bool finished() const;

    /// @brief Define o outro endpoint que vai receber os dados.
    /// @param receiver O outro endpoint
    void setReceiver(Receiver* receiver);

    /// @brief Define o enlace que o endpoint esta localizado
    /// @param channel o enlace atual
    void setChannel(Channel* channel);

    /// @brief Recebe um frame do outro endpoint. Deve ser
    /// adicionado na fila(buffer);
    /// @param frame 
    void receiveAck(const Frame& frame);
    
    /// @brief Roda um tick de atualizacao deste endpoint.
    /// Deve enviar confirmacoes se recebeu algo ou enviar
    /// um quadro se tiver algo para enviar.
    void update();

    uint64_t address = 0;
private:
    Receiver* receiver = nullptr;
    Channel* channel = nullptr;
    std::vector<Frame> data;
    std::queue<Frame> windowBuffer;
    int currenttick = 0;
    int windowSize = 8 - 1;
    int base = 0;
    int nextSeqNum = 1;

    std::unordered_map<int, std::chrono::steady_clock::time_point> timeSentMap; // Mapa de tempos de envio
    const int timeoutInterval = 100; // Intervalo de timeout em milissegundos

};

#endif