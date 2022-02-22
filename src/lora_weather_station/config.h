//#pragma once

// * Data Resolution *
#define RECORD_RESOLUTION 10

// * Records per LoRa packet *
#define RECORD_AMOUNT 1

// * LoRa Frequency *
// please see https://github.com/dragino/arduino-lmic#configuration
// the frequency must be set in the lmic libraries config file in 'Arduino/libraries/arduino-lmic-master/src/lmic/config.h', not in this application!

// * LoRa Spreading Factor *
#define SPREADING_FACTOR DR_SF12 // Options: DR_SF7, DR_SF8, DR_SF9, DR_SF10, DR_SF11, DR_SF12