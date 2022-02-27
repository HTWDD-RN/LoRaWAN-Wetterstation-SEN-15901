// * Usage Example *
/* You want to receive weather updates every half an hour. A packet contains 6 records with each representing the averaged data for 5 minutes.
 * RECORD_RESOLUTION: 360   // 60 * 5 seconds --> every 5 minutes a record will be added to the packet
 * RECORD_AMOUNT: 6         // after 6 records in cache, the packet will be sent over LoRa
 * SPREADING_FACTOR: DR_SF9
 *
 * NOTE: You should calculate the airtime of your configuration to meet the TTN fair usage policy.
 * Calculating the Airtime via https://www.thethingsnetwork.org/airtime-calculator
 * Input Bytes: RECORD_AMOUNT * 3 Bytes = 36 Bytes
 * Spreading Factor = SF9
 * Region: EU868
 * Bandwidth: 125khz
 *
 * Result: 246.8 ms
 * Airtime for a whole day: 246.8 ms * 2 * 24 ≈ 11.8 seconds
 * According to TTN fair use policy an average of 30 seconds per 24 hours should be maintained.
 *
 * NOTE: The more records a packet contains the more efficient is the data-to-airtime ratio, since the LoRaWan overhead is static
 * TODO: check if this is true, when sending packets with interval of 5 minutes
 * NOTE: Please check that you send packages to TTN with an interval of several minutes, otherwise the network might ignore them
*/

// * Data Resolution *
// e.g. a value of 10 means the average of 10 seconds will create 1 record
// minimum is 1 second
#define RECORD_RESOLUTION 60

// * Records per LoRa packet *
// 1 record = 3 bytes
#define RECORD_AMOUNT 20

// * LoRa Frequency *
// please see https://github.com/dragino/arduino-lmic#configuration
// the frequency must be set in the lmic libraries config file in 'home/Arduino/libraries/arduino-lmic-master/src/lmic/config.h', not in this application!

// * LoRa Spreading Factor *
#define SPREADING_FACTOR DR_SF7 // Options: DR_SF7, DR_SF8, DR_SF9, DR_SF10, DR_SF11, DR_SF12

// * Bandwidth *
// default is set to 125kHz, we found no setting to change it with the Lora Shield