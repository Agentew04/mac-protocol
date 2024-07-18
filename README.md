# mac-protocol
Implementação de um protocolo fictício para controle de acesso ao enlace de dados. Feito para disciplina ELC123

## Features

* Quadros de tamanho variável.
* Bit de Paridade para detecção de erro no header
e CRC para detecção de erro no payload (controle de erro).
* Mecanismo de controle de fluxo: Go-Back-N ARQ (controle de fluxo).
* Piggy backing das mensagens de controle.
* Padding nos dados.

## Estrutura do Quadro

| Nome do Campo         | Tamanho | Valor(se aplicável) | Funcionalidade   |
|-----------------------|---------|---------------------|------------------|
| Start Frame Delimiter | 8 bits | 0xCA | Define o início do quadro |
| Payload Length | 8 bits | 0-255 | Tamanho do payload em bits dividido por 4 |
| Endereço Transmissor | 48 bits | - | Endereço MAC do transmissor |
| Endereço Receptor    | 48 bits | - | Endereço MAC do destinatário |
| Payload Frame Number | 6 bits | 0-63 | Número do quadro |
| ACK | 1 bit | 0 ou 1 | Indica se o próximo campo define o ultimo quadro recebido |
| ACK Number | 6 bits | 0-63 | Número do quadro recebido ou nada de ACK for 0 |
| CRC | 16 bits | - | Código de detecção de erro no payload |
| Parity Bit | 1 bit | 0 ou 1 | Bit de paridade para detecção de erro no header. Inclui o CRC no cálculo |
| Padding Size | 2 bits | 0-3 | Tamanho do padding a ser adicionado ao payload |
| Payload | 0-255 bits | - | Dados a serem transmitidos. Deve ser um múltiplo de 4. Se não for recebe um padding para completar de acordo com `PaddingSize` |
| End Frame Delimiter | 8 bits | 0xFE | Define o fim do quadro |

O tamanho mínimo de um quadro é de 40

### Tamanhos e Offsets

- start frame delimiter (1 byte) = 0-7
- payload length (1 byte) = 8-15
- transmitter address (48 bits) = 16-63
- receiver address (48 bits) = 64-111
- payload frame number (6 bits) = 112-117
- ack (1 bit) = 118
- ack number (6 bits) = 119-124
- crc (16 bits) = 125-140
- parity bit (1 bit) = 141
- padding size (2 bits) = 142-143
- payload (0-1020 bits) = 144-x (multiplos de 4)
- end frame delimiter (1 byte) = x+1-x+8