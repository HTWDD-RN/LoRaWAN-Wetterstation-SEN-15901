#include <string.h>


#define SPEED_PER_CLICK 2.4     //km/h
#define RAIN_PER_CLICK  0.2794  //mm - 1.54 ml water
// 1mm - 5.5 ml

#define SPEED_PIN PCINT20 // pin 4
#define RAIN_PIN PCINT21  // pin 5

int windVanePin = A0;
int directionIndex = 0;
float windVaneVoltage = 0.0;

typedef struct WindDirection {
  float voltage;
  float angle;
  String direction;
} windDirection;

windDirection directions[] = {
  { 0.32, 112.5, "ESE" },
  { 0.41,  67.5, "ENE" },
  { 0.45,  90.0,   "E" },
  { 0.62, 157.5, "SSE" },
  {  0.9, 135.0,  "SE" },
  { 1.19, 202.5, "SSW" },
  {  1.4, 180.0,   "S" },
  { 1.98,  22.5, "NNE" },
  { 2.25,  45.0,  "NE" },
  { 2.93, 247.5, "WSW" },
  { 3.08, 225.0,  "SW" },
  { 3.43, 337.5, "NNW" },
  { 3.84,   0.0,   "N" },
  { 4.04, 292.5, "WNW" },
  { 4.33, 315.0,  "NW" },
  { 4.62, 270.0,   "W" },
};

volatile bool speedRead = false;
volatile int rawWindSpeedValue = 0;
volatile int rawRainValue = 0;      // variable to store the read value - volatile to push the update through

// variables that should be set once per second thus during packet creation / serial print we will not use changing values
volatile bool valuesRead = false;
volatile int currentWindSpeedvalue = 0;
volatile int currentRainValue = 0;

unsigned int counterStart = 3036;

static unsigned long last_interrupt_time_speed = 0;
static unsigned long last_interrupt_time_rain = 0;
unsigned long interrupt_time_speed = 0;
unsigned long interrupt_time_rain = 0;
unsigned int maxDirectionsIndex = (sizeof (directions) / sizeof (directions[0]));

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

  Serial.begin(9600);       //initialize serial monitoring
}


void loop() {
  
  // busy wait - is ok since all the interrupts have a higher priority and are not blocked
  // also mutex libraries cannot be included
  // the following block should not take more than 1s
  if (valuesRead) {
    valuesRead = false;

    windVaneVoltage = analogRead(windVanePin) * (5.0 / 1023);

    
    //search the sorted array - break if we find a higher value or found the last one
    for (directionIndex = 0; directionIndex < maxDirectionsIndex - 1; directionIndex++) {
      if (windVaneVoltage <= directions[directionIndex].voltage) {
        break;
      }
    }

    // which direction is closer to windVaneVoltage?
    if (directionIndex != 0) {
      if ((directions[directionIndex].voltage - windVaneVoltage) > (windVaneVoltage - directions[directionIndex - 1].voltage)) {
        directionIndex--;
      } 
    }

    Serial.print("Wind Direction: U = ");
    Serial.print(windVaneVoltage);
    Serial.print(" V, ");
    Serial.print(directions[directionIndex].angle);
    Serial.print(" ??, ");
    Serial.print(directions[directionIndex].direction);
    Serial.print(" ; Speed: ");
    Serial.print(currentWindSpeedvalue * SPEED_PER_CLICK);
    Serial.print("km/h ; Rain: ");
    Serial.print(currentRainValue * RAIN_PER_CLICK);
    Serial.println("mm");
    
    // Serial.print("PIND: ");
    // Serial.println(PIND, BIN);

    // Serial.print("PIN D4: ");
    // // beacuse of the pullup we have to check if the pin is low, to know if it was triggered
    // Serial.println((PIND & (1 << SPEED_PIN)) == 0, BIN);

    // Serial.print("PIN D5: ");
    // Serial.println((PIND & (1 << RAIN_PIN)) == 0, BIN);
  }
}

// loop every second
ISR (TIMER1_OVF_vect) {
  TCNT1 = counterStart; // adjust register value for 1 second overflow

  // read current values into Sec variables for asyn output in loop
  currentRainValue = rawRainValue;
  currentWindSpeedvalue = rawWindSpeedValue;

  // reset
  // lost update is possible, if the pin change interrupt interrupt this interrupt (pin change has higher priority)
  // thus some counts may get lost, however the reset  itself cannot be lost - lower priority - which is what we want
  rawRainValue = 0;
  rawWindSpeedValue = 0;
  
  valuesRead = true;  //enable loop to enter
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

    //implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
    if (interrupt_time_speed - last_interrupt_time_speed > 10) {  //min time between readings - TODO maybe change value
      rawWindSpeedValue++;
      last_interrupt_time_speed = interrupt_time_speed;
    }

    speedRead = true;
  }


  if ((PIND & (1 << RAIN_PIN)) == 0) {
    interrupt_time_rain = millis();

    //implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
    if (interrupt_time_rain - last_interrupt_time_rain > 100) { // max 27 clicks per 10 seconds => 370 ms between readings
      rawRainValue++;
      last_interrupt_time_rain = interrupt_time_rain;
    }
  }
}