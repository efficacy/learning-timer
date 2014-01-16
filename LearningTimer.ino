/*
  Learning Timer
  
 No fiddling with switches and displays to set how long to wait, just teach the timer 
 by clicking the LEARN button, waiting, and clicking it again. Once you have taugyht the
 timer, you can run it for that duration any time you like by pressing the RUN button. 
 At the end of the learned time, the buzzer wil sound. Simple
 
 The circuit:
 * LEARN button (active HIGH) attached to pin 8
 * LEARN button (active HIGH) attached to pin 9
 * Piezo buzzer attached to pin 10 through a resistor
 * optional 8-LED array attached to pins 0-7
 
 */

// constants won't change. They're used here to 
// set pin numbers:
const int learnButton = 8;
const int runButton = 9;
const int buzzPin =  10;
enum { WAITING, LEARN_START_PRESS, LEARNING, LEARN_END_PRESS, RUN_PRESS, RUNNING } state;

const unsigned int tick = 100; // ms
const unsigned int ticksPerSecond = 1000 / tick;
const unsigned int buzzFreq = 1000;
const unsigned long buzzDuration = 500;

long goal;
long count;

void setup() {
  for (int i = 0; i < 8; ++i) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(learnButton, INPUT);
  pinMode(runButton, INPUT);
  pinMode(buzzPin, OUTPUT);
  
  goal = 10 * ticksPerSecond; // for now, delay 10s
  count = 0;
  state = WAITING;
  
  Serial.begin(9600); // send and receive at 9600 baud
  Serial.println("start");
}

void loop(){
  delay(tick);
  ++count;
  for (int i = 0; i < 8; ++i) digitalWrite(i, HIGH);
  
  switch (state) {
    case WAITING:
      digitalWrite(0, LOW);
      if (digitalRead(learnButton) == HIGH) {
        Serial.println("WAITING, press LEARN");
        state = LEARN_START_PRESS;
      } else if (digitalRead(runButton) == HIGH) {
        Serial.println("WAITING, press RUN");
        state = RUN_PRESS;
      }
      break;
    case LEARN_START_PRESS:
      digitalWrite(1, LOW);
      if (digitalRead(learnButton) == LOW) {
        Serial.println("LEARN_START_PRESS, release LEARN");
        state = LEARNING;
        count = 0;
      }
    case LEARNING:
      digitalWrite(2, LOW);
      if (digitalRead(learnButton) == HIGH) {
        Serial.println("LEARNING, press LEARN");
        state = LEARN_END_PRESS;
      }
      break;
    case LEARN_END_PRESS:
      digitalWrite(3, LOW);
      if (digitalRead(learnButton) == LOW) {
        Serial.println("LEARN_END_PRESS, release LEARN");
        Serial.print("learned ");
        Serial.print(count);
        Serial.println(" ticks");
        state = WAITING;
        goal = count;
        count = 0;
      }
      break;
    case RUN_PRESS:
      digitalWrite(4, LOW);
      if (digitalRead(runButton) == LOW) {
        Serial.println("RUN_PRESS, release RUN");
        state = RUNNING;
        count = 0;
      }
      break;
    case RUNNING:
      digitalWrite(5, LOW);
      if (count >= goal) {
        Serial.print("ran ");
        Serial.print(count);
        Serial.println(" ticks");
        state = WAITING;
        tone(buzzPin, buzzFreq, buzzDuration);
        count = 0;
      }
      break;
  }
}
