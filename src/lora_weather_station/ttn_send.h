#include <stdint.h>
#include "config.h"

typedef uint8_t byte;

extern byte cache[3 * RECORD_AMOUNT];

void doSend();
void setupLoRa();
void loopLoRa();