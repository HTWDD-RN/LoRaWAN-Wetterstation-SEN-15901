// This file is a template with ABP/OTAA-Secrets for the LoRa end-device.
// Please copy this file as "secret.h" and fill in the necessary secrets from TTN to identify your LoRa end-device.

#include "config.h"

#ifdef OTAA_MODE
    // NOTE: JOINEUI and DEVEUI need to be in little-endian format
    #define JOINEUI_SECRET { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } // formerly called AppEUI; little-endian format
    #define DEVEUI_SECRET { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } // little-endian format
    #define APPKEY_SECRET { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#elif defined(ABP_MODE)
    #define NWKSKEY_SECRET { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    #define APPSKEY_SECRET { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    #define DEVADDR_SECRET 0x00000000
#endif