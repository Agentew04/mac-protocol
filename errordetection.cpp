#include "errordetection.h"



uint16_t crc16(char *data_p, size_t length){
    uint16_t crc = 0xFFFF; // Valor inicial do CRC16
    uint16_t polynomial = 0x1021; // Polinômio CRC16-CCITT

    for (unsigned int i = 0; i < length; i++) {
        // Obtém o bit atual
        uint8_t bit = data_p[i];
        
        // Desloca o bit para o CRC
        crc ^= (bit << 15);

        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

int parity(char *data_p, size_t length){
    int count = 0;
    for (size_t i = 0; i < length; i++) {
        if (data_p[i] == 1) {
            count++;
        }
    }
    return (count % 2 == 0) ? 0 : 1;
}
