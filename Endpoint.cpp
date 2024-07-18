#include "Endpoint.h"

#include <iostream>
#include <vector>
#include <queue>

#include "Frame.h"
#include "Channel.h"

/// @brief Cria um endpoint que quer enviar dados.
/// @param data Os quadros que vao ser enviados
Endpoint::Endpoint(std::vector<Frame>& data){
    for(auto &d : data){
        outgoingBuffer.push(d);
    }
}

/// @brief Cria um endpoint que nao envia dados.
/// Apenas recebe as confirmações.
Endpoint::Endpoint(){
}

/// @brief Se o endpoint enviou tudo que precisava e
/// recebeu todos as confirmações.
/// @return true se acabou ou false se ainda nao
bool Endpoint::finished() const{
    return outgoingBuffer.empty() && incomingBuffer.empty();
}

/// @brief Define o outro endpoint que vai receber os dados.
/// @param receiver O outro endpoint
void Endpoint::setReceiver(Endpoint* receiver){
    this->receiver = receiver;
}

/// @brief Define o enlace que o endpoint esta localizado
/// @param channel o enlace atual
void Endpoint::setChannel(Channel* channel){
    this->channel = channel;
}

/// @brief Recebe um frame do outro endpoint. Deve ser
/// adicionado na fila(buffer);
/// @param frame
void Endpoint::receive(const Frame& frame){
    incomingBuffer.push(frame);
}

/// @brief Roda um tick de atualizacao deste endpoint.
/// Deve enviar confirmacoes se recebeu algo ou enviar
/// um quadro se tiver algo para enviar.
void Endpoint::update(){
    if(!outgoingBuffer.empty()){
        Frame frame = outgoingBuffer.front();
        outgoingBuffer.pop();
        channel->pass(frame);
    }

    if(!incomingBuffer.empty()){
        Frame frame = incomingBuffer.front();
        incomingBuffer.pop();
        receiver->receive(frame);
    }
}

