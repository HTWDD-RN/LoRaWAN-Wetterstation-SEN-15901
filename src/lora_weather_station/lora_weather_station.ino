#include "config.h"
#include "ttn_send.h"


#define SPEED_PIN PCINT20 // pin 4
#define RAIN_PIN PCINT21  // pin 5

byte cache[3 * RECORD_AMOUNT] = { 0 }; 

int windVanePin = A0;
int directionIndex = 0;
float windVaneVoltage = 0.0;

float directionVoltages[] = {
  0.32, 0.41, 0.45, 0.62, 0.9, 1.19, 1.4, 1.98, 2.25, 2.93, 3.08, 3.43, 3.84, 4.04, 4.33, 4.62
};

const int directionsLength = sizeof (directionVoltages) / sizeof (float);

volatile bool speedRead = false;
volatile int rawWindSpeedValue = 0;
volatile int rawRainValue = 0;      // variable to store the read value (volatile to push the update through)

// variables that should be set once per second thus during packet creation / serial print we will not use changing values
volatile bool valuesRead = false;
volatile int currentWindSpeedValue = 0;
volatile int currentRainValue = 0;

// variables that will store the record averages
unsigned int secCount = 0;
unsigned int recordCount = 0;
unsigned int avgWindSpeedValue = 0;
unsigned int maxWindSpeedValue = 0;
unsigned int rainAmount = 0;
unsigned int maxDirectionIndex = 0;

// take track of the most occuring wind directions for averaged record
int windDirectionStats[directionsLength] = { 0 };


unsigned int counterStart = 3036;

static unsigned long last_interrupt_time_speed = 0;
static unsigned long last_interrupt_time_rain = 0;
unsigned long interrupt_time_speed = 0;
unsigned long interrupt_time_rain = 0;

void setup() {

  cli(); // disable all interrupts
  
  // init digital pins
  PCICR |= (1 << PCIE2);    // enable interrupt for PCINT20 (D0 to D7)

  PCMSK2 |= (1 << SPEED_PIN); // set digital pin 4 
  PCMSK2 |= (1 << RAIN_PIN); // set digital pin 5

  pinMode(SPEED_PIN, INPUT_PULLUP);
  pinMode(RAIN_PIN, INPUT_PULLUP);

  // init timer
  TCCR1A = 0x00;
  TCCR1B = (1 << CS12); // prescaler = 256; alternative: 1024 (set CS12 and CS10)
  TIMSK1 = (1 << TOIE1); // interrupt when TCNT1 is overflowed
  TCNT1 = counterStart;

  sei(); // enable interrupts     

  Serial.begin(9600); // initialize serial monitoring

  setupLoRa();
}


void loop() {
  
  // busy wait - is ok since all the interrupts have a higher priority and are not blocked
  // also mutex libraries cannot be included
  // the following block should not take more than 1s
  if (valuesRead) {
    valuesRead = false;

    windVaneVoltage = analogRead(windVanePin) * (5.0 / 1023);

    
    // search the sorted array - break if we find a higher value or found the last one
    for (directionIndex = 0; directionIndex < directionsLength - 1; directionIndex++) {
      if (windVaneVoltage <= directionVoltages[directionIndex]) {
        break;
      }
    }

    // which direction is closer to windVaneVoltage?
    if (directionIndex != 0) {
      if ((directionVoltages[directionIndex] - windVaneVoltage) > (windVaneVoltage - directionVoltages[directionIndex - 1])) {
        directionIndex--;
      } 
    }

    // calculate cumulative sum for averaged records
    windDirectionStats[directionIndex]++;
    avgWindSpeedValue += currentWindSpeedValue;
    if (currentWindSpeedValue > maxWindSpeedValue) maxWindSpeedValue = currentWindSpeedValue;
    rainAmount += currentRainValue;
    secCount++;

    if (secCount >= RECORD_RESOLUTION) {
      
      // check for the most occurred wind direction
      maxDirectionIndex = 0;
      for (directionIndex = 1; directionIndex < directionsLength; directionIndex++) {
        // if there are 2 wind directions that occurred equally, just take the first one
        if (windDirectionStats[maxDirectionIndex] < windDirectionStats[directionIndex]) {
          maxDirectionIndex = directionIndex;
        }
      }

      avgWindSpeedValue /= RECORD_RESOLUTION;

      // byte representation of a record, that will be sent over LoRa

      /* [      0]  [      1]  [      2] */ // byte count
      /* 0000 0000  0000 0000  0000 0000 */ // bit count
      /* [WV] [..AN..][.M_AN]  [...R...] */ // value allocation
      // WV = windVaneDirectionIndex, R = rainAmount, AN = avgWindSpeed, M_AN = maxWindSpeed

      // check if the wind speed and rain amount values exceed the intended bit space
      // if yes, set them on the available maximum
      if (avgWindSpeedValue > 0x3F) avgWindSpeedValue = 0x3F; 
      if (maxWindSpeedValue > 0x3F) maxWindSpeedValue = 0x3F;
      if (rainAmount > 0xFF) rainAmount = 0xFF;

      // add current record to cache
      cache[recordCount] = maxDirectionIndex << 4;
      cache[recordCount] |= (avgWindSpeedValue >> 2) & 0x0F;

      cache[recordCount + 1] = avgWindSpeedValue << 6;
      cache[recordCount + 1] |= maxWindSpeedValue & 0x3F;

      cache[recordCount + 2] = rainAmount;

      // reset
      avgWindSpeedValue = 0;
      maxWindSpeedValue = 0;
      rainAmount = 0;

      // reset the windDirectionStats Array to 0
      memset(windDirectionStats, 0, sizeof(windDirectionStats));


      Serial.println("");
      for (int i = 0; i < RECORD_AMOUNT * 3; i++) {
        Serial.println(cache[i], BIN);
      }

      secCount = 0;
      recordCount += 3;
    }
    
    // send LoRa packet
    if ((recordCount / 3) >= RECORD_AMOUNT) {
      
      doSend();

      recordCount = 0;

      // reset cache
      memset(cache, 0, sizeof(cache));
    }
  }

  loopLoRa();
}

// loop every second
ISR (TIMER1_OVF_vect) {
  TCNT1 = counterStart; // adjust register value for 1 second overflow

  // write current values into variables that will not change during record creation
  currentRainValue = rawRainValue;
  currentWindSpeedValue = rawWindSpeedValue;

  // reset
  // lost update is possible, if the pin change interrupt interrupts this interrupt (pin change has higher priority)
  // thus some counts may get lost, however the reset  itself cannot be lost - lower priority - which is what we want
  rawRainValue = 0;
  rawWindSpeedValue = 0;
  
  valuesRead = true; // enable loop to enter
}


// handle digital interrupts (rain gauge and anemometer)
ISR (PCINT2_vect) {

  // Serial.print("PIND: ");
  // Serial.println(PIND, BIN);

  // Serial.print("PIN D4: ");
  // // because of the pullup we have to check if the pin is low, to know if it was triggered
  // Serial.println((PIND & (1 << SPEED_PIN)) == 0, BIN);

  // Serial.print("PIN D5: ");
  // Serial.println((PIND & (1 << RAIN_PIN)) == 0, BIN);

  // if the switch in anemometer remains in closed position, the D4 pin is always high (anemometer does not move)
  // problem: if the rain sensor is triggered, while D4 (speed pin) is constantly high, rawWindSpeedValue and rawRainValue would be incremented, but anemometer did not move
  // solution: after the anemometer triggered an interrupt, lock the ISR for speed updates, until the pin state changes to low
  // then, unlock the ISR for speed updates, when the pin state changes to high

  if ((PIND & (1 << SPEED_PIN)) != 0) { // speed pin released (D4 = 0)
    speedRead = false;
  }


  if (((PIND & (1 << SPEED_PIN)) == 0) && !speedRead) { // speed pin pressed (D4 = 1)
    interrupt_time_speed = millis();

    // implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
    if (interrupt_time_speed - last_interrupt_time_speed > 10) {  // min time between readings - TODO maybe change value
      rawWindSpeedValue++;
      last_interrupt_time_speed = interrupt_time_speed;
    }

    speedRead = true;
  }


  if ((PIND & (1 << RAIN_PIN)) == 0) {
    interrupt_time_rain = millis();

    // implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
    if (interrupt_time_rain - last_interrupt_time_rain > 100) { // max 27 clicks per 10 seconds => 370 ms between readings
      rawRainValue++;
      last_interrupt_time_rain = interrupt_time_rain;
    }
  }
}