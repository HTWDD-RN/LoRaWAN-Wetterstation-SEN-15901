int ledPin = LED_BUILTIN;  // LED connected to digital pin 13
int inPin = 7;    // pushbutton connected to digital pin 7
int val = 0;      // variable to store the read value
long count = 0;

void setup() {
  pinMode(ledPin, OUTPUT);  // sets the digital pin 13 as output
  pinMode(inPin, INPUT);    // sets the digital pin 7 as input

  Serial.begin(9600);       //initialize serial monitoring
}

void loop() {
  val = digitalRead(inPin);   // read the input pin - does not block!
  digitalWrite(ledPin, val);  // sets the LED to the button's value
  if (val) {
    count++;
    Serial.println(count);
  }
}