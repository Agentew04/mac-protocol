#include "Channel.h"

Channel::Channel() {
    gen = std::mt19937(rd());
    dropDistribution = std::uniform_real_distribution<double>(0, 1);
    bitDistribution = std::uniform_real_distribution<double>(0, 1);
}

bool Channel::shouldDrop() {
    return dropDistribution(gen) < dropThreshold;
}

Frame Channel::pass(Frame frame) {
    std::vector<bit> bits = frame.toBits();
    for (int i = 0; i < bits.size(); i++) {
        if (bitDistribution(gen) < bitErrorRate) {
            if(bits[i] == 0){
                bits[i] = 1;
            }else{
                bits[i] = 0;
            }
        }
    }
    return Frame::fromBits(bits);
}