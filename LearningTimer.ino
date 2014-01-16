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

void clear_dots() {
  for (int i = 0; i < 8; ++i) {
    digitalWrite(i, HIGH);
  }
}  

// 'hop' a LED along to show learning state
int dot = 0;
void hop() {
  for (int i = 0; i < 8; ++i) digitalWrite(i, i == dot ? LOW : HIGH);
  dot = (dot + 1) % 8;
}

void bar() {
  float progress = count / (goal * 1.0);
  int dots = (int)(8 * progress);
  for (int i = 0; i < 8; ++i) digitalWrite(i, i < dots ? HIGH : LOW);
}

void setup() {
//  Serial.begin(9600); // send and receive at 9600 baud
//  Serial.println("start");

  for (int i = 0; i < 8; ++i) {
    pinMode(i, OUTPUT);
  }
  clear_dots();
  
  pinMode(learnButton, INPUT);
  pinMode(runButton, INPUT);
  pinMode(buzzPin, OUTPUT);
  
  goal = 10 * ticksPerSecond; // for now, delay 10s
  count = 0;
  state = WAITING;
}

void loop(){
  delay(tick);
  ++count;
  
  switch (state) {
    case WAITING:
      if (digitalRead(learnButton) == HIGH) {
//        Serial.println("WAITING, press LEARN");
        state = LEARN_START_PRESS;
      } else if (digitalRead(runButton) == HIGH) {
//        Serial.println("WAITING, press RUN");
        state = RUN_PRESS;
      }
      break;
    case LEARN_START_PRESS:
      if (digitalRead(learnButton) == LOW) {
//        Serial.println("LEARN_START_PRESS, release LEARN");
        state = LEARNING;
        count = 0;
      }
    case LEARNING:
      hop();
      if (digitalRead(learnButton) == HIGH) {
//        Serial.println("LEARNING, press LEARN");
        state = LEARN_END_PRESS;
      }
      break;
    case LEARN_END_PRESS:
      if (digitalRead(learnButton) == LOW) {
//        Serial.println("LEARN_END_PRESS, release LEARN");
//        Serial.print("learned ");
//        Serial.print(count);
//        Serial.println(" ticks");
        state = WAITING;
        goal = count;
        count = 0;
        clear_dots();
      }
      break;
    case RUN_PRESS:
      if (digitalRead(runButton) == LOW) {
//        Serial.println("RUN_PRESS, release RUN");
        state = RUNNING;
        count = 0;
      }
      break;
    case RUNNING:
      bar();
      if (count >= goal) {
//        Serial.print("ran ");
//        Serial.print(count);
//        Serial.println(" ticks");
        state = WAITING;
        tone(buzzPin, buzzFreq, buzzDuration);
        count = 0;
      }
      break;
  }
}
