/*******************************************************************************
 * Copyright (c) 2019 Thomas Telkamp and Matthijs Kooijman
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * Modified by Lenny Reitz and Raphael Rezaii-Djafari
 *******************************************************************************/

#include "ttn_send.h"
#include "secret.h"
#include "config.h"

#include <string.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// check if ABP or OTAA should be used, OTAA is recommend
// identification modes are defined in config.h
#if defined(OTAA_MODE)
  static const u1_t PROGMEM JOINEUI[8] = JOINEUI_SECRET;
  static const u1_t PROGMEM DEVEUI[8] = DEVEUI_SECRET;
  static const u1_t PROGMEM APPKEY[16] = APPKEY_SECRET;

  void os_getArtEui (u1_t* buf) { memcpy_P(buf, JOINEUI, 8); }
  void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
  void os_getDevKey (u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

#elif defined(ABP_MODE)
  static const PROGMEM u1_t NWKSKEY[16] = NWKSKEY_SECRET;
  static const u1_t PROGMEM APPSKEY[16] = APPSKEY_SECRET;
  static const u4_t DEVADDR = DEVADDR_SECRET;

  void os_getArtEui (u1_t* buf) { }
  void os_getDevEui (u1_t* buf) { }
  void os_getDevKey (u1_t* buf) { }
#endif

static osjob_t sendjob;

const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 6, 7},
};

void do_send(osjob_t * j) {
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } 
  else {
    LMIC_setTxData2(1, cache, sizeof(cache), 0);
    Serial.println(F("Packet queued"));
  }
}

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  switch(ev) {
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));

      #ifdef OTAA_MODE
        // Disable link check validation (automatically enabled
        // during join, but not supported by TTN at this time).
        LMIC_setLinkCheckMode(0);
      #endif

      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if(LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ACK"));
      if(LMIC.dataLen) {
        Serial.print(F("Received "));
        Serial.print(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
      }
      break;
    case EV_RXCOMPLETE:
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}


void doSend() {
  Serial.println(F("Sending"));
  do_send(&sendjob);
}


void setupLoRa() {
  Serial.begin(9600);
  Serial.println(F("Starting"));

  // identification mode check
  #if defined(OTAA_MODE)
    Serial.println(F("OTAA mode"));
  #elif defined(ABP_MODE)
    Serial.println(F("ABP mode"));
  #endif

  #ifdef VCC_ENABLE
    pinMode(VCC_ENABLE, OUTPUT);
    digitalWrite(VCC_ENABLE, HIGH);
    delay(1000);
  #endif

  os_init();
  LMIC_reset();

  #ifdef ABP_MODE
    #ifdef PROGMEM
      uint8_t appskey[sizeof(APPSKEY)];
      uint8_t nwkskey[sizeof(NWKSKEY)];
      memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
      memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
      LMIC_setSession(0x1, DEVADDR, nwkskey, appskey);
    #else
      LMIC_setSession(0x1, DEVADDR, NWKSKEY, APPSKEY);
    #endif
  

    #if defined(CFG_eu868)
      LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
      LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
      LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    #elif defined(CFG_us915)
      // NA-US channels 0-71 are configured automatically
      // but only one group of 8 should (a subband) should be active
      // TTN recommends the second sub band, 1 in a zero based count.
      // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json

      LMIC_selectSubBand(1);
    #endif

    LMIC_setLinkCheckMode(0);
    LMIC.dn2Dr = DR_SF9; // downlink SF is fix, because TTN only uses SF9 for its RX2 window
    LMIC_setDrTxpow(SPREADING_FACTOR, 14);
  #endif
}

void loopLoRa() {
  os_runloop_once();
}