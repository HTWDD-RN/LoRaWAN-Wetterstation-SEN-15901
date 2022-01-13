#include <stdint.h>

typedef uint8_t byte;

extern byte cache[30][3];

void doSend();
void setupLoRa();
void loopLoRa();