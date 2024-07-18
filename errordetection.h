#ifndef __ERROR_DETECTION_H__
#define __ERROR_DETECTION_H__

#include <cstdint>
#include <cstddef>
#include <stdlib.h>

/// @brief Calcula o CRC16 de uma sequencia binaria de dados. Opera bit a bit.
/// @param data_p Ponteiro para os dados. Cada char representa um BIT nao um byte.
/// @param length O numero de bits da sequencia
/// @return O valor do CRC16
uint16_t crc16(char *data_p, size_t length);


/// @brief Calcula o bit de paridade PAR referente aos dados recebidos. Opera bit a bit
/// @param data_p Ponteiro para os dados. Cada char representa um BIT nao um byte.
/// @param length O numero de bits da sequencia
/// @return 1 se o bit de paridade for 1, 0 se for 0
int parity(char *data_p, size_t length);

#endif