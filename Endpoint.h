#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

#include <vector>
#include <queue>

#include "Frame.h"
#include "Channel.h"

/// @brief Classe que representa uma ponta do enlace.
/// Transmite dados, espera ACKs. Recebe dados e envia ACKs.
class Endpoint {
public:

    /// @brief Cria um endpoint que quer enviar dados.
    /// @param data Os quadros que vao ser enviados
    Endpoint(std::vector<Frame>& data);
    /// @brief Cria um endpoint que nao envia dados.
    /// Apenas recebe as confirmações.
    Endpoint();


    /// @brief Se o endpoint enviou tudo que precisava e
    /// recebeu todos as confirmações.
    /// @return true se acabou ou false se ainda nao
    bool finished() const;

    /// @brief Define o outro endpoint que vai receber os dados.
    /// @param receiver O outro endpoint
    void setReceiver(Endpoint* receiver);

    /// @brief Define o enlace que o endpoint esta localizado
    /// @param channel o enlace atual
    void setChannel(Channel* channel);

    /// @brief Recebe um frame do outro endpoint. Deve ser
    /// adicionado na fila(buffer);
    /// @param frame 
    void receive(const Frame& frame);
    
    /// @brief Roda um tick de atualizacao deste endpoint.
    /// Deve enviar confirmacoes se recebeu algo ou enviar
    /// um quadro se tiver algo para enviar.
    void update();

    uint64_t address = 0;
private:
    Endpoint* receiver = nullptr;
    Channel* channel = nullptr;
    std::queue<Frame> incomingBuffer;
    std::vector<Frame> data;
    

    // variaveis da implementacao vao aqui
    std::queue<Frame> outgoingBuffer;
    

    std::vector<Frame> data;
    // go back n
    int windowSize = 64 - 1; // 2^6 - 1 = 63
    int nextFrameToSend = 0;
    int lastAckReceived = -1;
    int timeoutTicks = 32;
    int lastAckTime = 0;
    int currentTick = 0;
};

#endif