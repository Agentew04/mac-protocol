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
            bits[i] = !bits[i];
        }
    }
    return Frame::fromBits(bits);
}