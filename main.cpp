#include <iostream>
#include "errordetection.h"

int main(void){
    // input aleatorio de bits
    char data[] = {
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        0, 0, 0, 0, 
        0, 0, 0, 1};
    unsigned int length = 16;
    uint16_t crc = crc16(data, length);
    std::cout << "CRC16: " << crc << std::endl;

    return 0;
}