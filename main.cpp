#include <iostream>
#include <vector>
#include <random>
#include <ctime>

#include "Endpoint.h"
#include "Frame.h"
#include "errordetection.h"

typedef unsigned char bit;

std::vector<bit> generateData(){
    std::mt19937 gen(time(nullptr));

    // numero de bits
    std::uniform_int_distribution<int> dist(3000, 5000);
    // bit aleatorio
    std::uniform_int_distribution<int> dist2(0, 1);

    std::vector<bit> data;
    int bits = dist(gen);
    data.reserve(bits);

    for(int i = 0; i < bits; i++){
        data.push_back(dist2(gen));
    }
    return data;
}

void printData(const std::vector<bit> &data){
    for(auto &d : data){
        std::cout << (int)d;
    }
    std::cout << std::endl;
}

int main(void){
    Channel enlace;

    // gerar os dados
    std::cout << "Generating data... ";
    std::vector<bit> data = generateData();
    std::cout << "Done (" << data.size() << " bits)!" << std::endl;

    // dividir os bits de dados nos frames(e montar os headers)
    auto frames = Frame::generateFrames(data);

    // simular a troca de dados na rede
    Endpoint transmissor = Endpoint(frames);
    Endpoint receptor = Endpoint();

    transmissor.setReceiver(&receptor);
    receptor.setReceiver(&transmissor);
    transmissor.setChannel(&enlace);
    receptor.setChannel(&enlace);

    int i = 0;
    while(!transmissor.finished()) {
        // a cada iteracao, atualiza um tick em
        // cada maquina
        if(i % 2 == 0){
            transmissor.update();
        } else {
            receptor.update();
        }
        i++;
    }

    //printData(data);
    return 0;
}