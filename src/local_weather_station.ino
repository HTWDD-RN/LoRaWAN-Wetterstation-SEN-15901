#define SPEED_PER_CLICK 2.4     //km/h
#define RAIN_PER_CLICK  0.2794  //mm - 1.54 ml water
// 1mm - 5.5 ml

int speedPin = 2;
int rainPin = 3;

volatile int speedCount = 0;
volatile int rainCount = 0;      // variable to store the read value - volatile to push the update through

static unsigned long last_interrupt_time_speed = 0;
static unsigned long last_interrupt_time_rain = 0;
unsigned long interrupt_time_speed = 0;
unsigned long interrupt_time_rain = 0;

void setup() {
  pinMode(speedPin, INPUT_PULLUP);
  pinMode(rainPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(speedPin), incrementSpeed, RISING);
  attachInterrupt(digitalPinToInterrupt(rainPin), incrementRain, RISING);      

  Serial.begin(9600);       //initialize serial monitoring
}

void loop() {
  delay(1000);  //allow the counters to be filled before reading them

  Serial.print("Speed: ");
  Serial.print(speedCount * SPEED_PER_CLICK);
  Serial.print("km/h ; Rain: ");
  Serial.print(rainCount * RAIN_PER_CLICK);
  Serial.println("mm");

  speedCount = 0;
  rainCount = 0;
}

//two functions needed as the ISR are not allowed to use input parameters
void incrementSpeed() {
  interrupt_time_speed = millis();

  //implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
  if (interrupt_time_speed - last_interrupt_time_speed > 10) {  //min time between readings - TODO maybe change value
    speedCount++;
  }
  last_interrupt_time_speed = interrupt_time_speed;
}

void incrementRain() {
  interrupt_time_rain = millis();

  //implement software debouncer as we can easily configure the time and HW-Debouncing did not work for us
  if (interrupt_time_rain - last_interrupt_time_rain > 100) { // max 27 clicks per 10 seconds => 370 ms between readings
    rainCount++;
  }
  last_interrupt_time_rain = interrupt_time_rain;
}