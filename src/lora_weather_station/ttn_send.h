#include <stdint.h>
#include "config.h"

typedef uint8_t byte;

extern byte cache[RECORD_AMOUNT][3];

void doSend();
void setupLoRa();
void loopLoRa();