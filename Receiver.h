#ifndef __RECEIVER_H__
#define __RECEIVER_H__

class Transmitter;

class Receiver {
public:
    /// @brief Define o outro endpoint que vai receber os dados.
    /// @param receiver O outro endpoint
    void setTransmitter(Transmitter* transmitter);

    /// @brief Define o enlace que o endpoint esta localizado
    /// @param channel o enlace atual
    void setChannel(Channel* channel);

    /// @brief Recebe um frame de dados do transmissor
    /// @param frame O frame a ser recebido
    void receiveFrame(const Frame& frame);

    /// @brief Roda um tick de atualizacao deste endpoint.
    /// Deve enviar confirmacoes se recebeu algo ou enviar
    /// um quadro se tiver algo para enviar.
    void update();

    uint64_t address = 0;
private:
    Channel* channel = nullptr;
    Transmitter* transmitter = nullptr;

    int awaitingNum = 0;
};

#endif