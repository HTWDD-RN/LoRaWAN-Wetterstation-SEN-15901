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
volatile int speedCount = 0;
volatile int rainCount = 0;      // variable to store the read value - volatile to push the update through

static unsigned long last_interrupt_time_speed = 0;
static unsigned long last_interrupt_time_rain = 0;
unsigned long interrupt_time_speed = 0;
unsigned long interrupt_time_rain = 0;
unsigned int maxDirectionsIndex = (sizeof (directions) / sizeof (directions[0]));

void setup() {

  cli(); // disable all interrupts
  
  PCICR |= (1 << PCIE2);    // enable interrupt for PCINT20 (D0 to D7)

  PCMSK2 |= (1 << SPEED_PIN); // set digital pin 4 
  PCMSK2 |= (1 << RAIN_PIN); // set digital pin 5

  pinMode(SPEED_PIN, INPUT_PULLUP);
  pinMode(RAIN_PIN, INPUT_PULLUP);

  sei(); // enable interrupts     

  Serial.begin(9600);       //initialize serial monitoring
}

void loop() {
  delay(1000);  //allow the counters to be filled before reading them

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
  Serial.print(" °, ");
  Serial.print(directions[directionIndex].direction);
  Serial.print(" ; Speed: ");
  Serial.print(speedCount * SPEED_PER_CLICK);
  Serial.print("km/h ; Rain: ");
  Serial.print(rainCount * RAIN_PER_CLICK);
  Serial.println("mm");
  
  // Serial.print("PIND: ");
  // Serial.println(PIND, BIN);

  // Serial.print("PIN D4: ");
  // // beacuse of the pullup we have to check if the pin is low, to know if it was triggered
  // Serial.println((PIND & (1 << SPEED_PIN)) == 0, BIN);

  // Serial.print("PIN D5: ");
  // Serial.println((PIND & (1 << RAIN_PIN)) == 0, BIN);

  speedCount = 0;
  rainCount = 0;

  
}

ISR (PCINT2_vect) {

  // Serial.print("PIND: ");
  // Serial.println(PIND, BIN);

  // Serial.print("PIN D4: ");
  // // because of the pullup we have to check if the pin is low, to know if it was triggered
  // Serial.println((PIND & (1 << SPEED_PIN)) == 0, BIN);

  // Serial.print("PIN D5: ");
  // Serial.println((PIND & (1 << RAIN_PIN)) == 0, BIN);

  // if the switch in anemometer remains in closed position, the D4 pin is always high (anemometer does not move)
  // problem: if the rain sensor is triggered, while D4 (speed pin) is constantly high, speedCount and rainCount would be incremented, but anemometer did not move
  // solution: after the anemometer triggered an interrupt, lock the ISR for speed updates, until the pin state changes to low
  // then, unlock the ISR for speed updates, when the pin state changes to high

  if ((PIND & (1 << SPEED_PIN)) != 0) { // speed pin released (D4 = 0)
    speedRead = false;
  }


  if (((PIND & (1 << SPEED_PIN)) == 0) && !speedRead) { // speed pin pressed (D4 = 1)
    interrupt_time_speed = millis();

    //implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
    if (interrupt_time_speed - last_interrupt_time_speed > 10) {  //min time between readings - TODO maybe change value
      speedCount++;
      last_interrupt_time_speed = interrupt_time_speed;
    }

    speedRead = true;
  }


  if ((PIND & (1 << RAIN_PIN)) == 0) {
    interrupt_time_rain = millis();

    //implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
    if (interrupt_time_rain - last_interrupt_time_rain > 100) { // max 27 clicks per 10 seconds => 370 ms between readings
      rainCount++;
      last_interrupt_time_rain = interrupt_time_rain;
    }
  }
}