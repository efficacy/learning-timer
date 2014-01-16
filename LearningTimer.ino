/*
  Learning Timer
  
 No fiddling with switches and displays to set how long to wait, just teach the timer 
 by clicking the LEARN button, waiting, and clicking it again. Once you have taught the
 timer, you can run it for that duration any time you like by pressing the RUN button. 
 At the end of the learned time, the buzzer wil sound. Simple.
 
 As a bonus, if you connect 8 LEDS to pins 0-7 they will indicate the state of the system.
 While learning, a dot will "walk" along the array. While running, a "solid" bar will
 steadily decrease until the buzzer sounds. It's worth it, it looks neat.

 The circuit:
 * LEARN button (active HIGH) attached to pin 8
 * RUN button (active HIGH) attached to pin 9
 * Piezo buzzer attached to pin 10 through a resistor
 * (optional) 8-LED array attached to pins 0-7
 
 Frank Carver, January 2014
 */

const int learnButton = 8;
const int runButton = 9;
const int buzzPin =  10;

// to avoid unintended very-short durations, times start from when you let go of the button
enum { WAITING, LEARN_START_PRESS, LEARNING, LEARN_END_PRESS, RUN_PRESS, RUNNING } state;

const unsigned int tick = 100; // ms
const unsigned int ticksPerSecond = 1000 / tick;
const unsigned int buzzFreq = 1000;
const unsigned long buzzDuration = 500;

long goal;
long count;
long next_tick;

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
  for (int i = 7; i >= 0; --i) digitalWrite(i, i < dots ? HIGH : LOW);
}

void setup() {
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
  next_tick = millis() + tick;
  
  switch (state) {
    case WAITING:
      if (digitalRead(learnButton) == HIGH) {
        state = LEARN_START_PRESS;
      } else if (digitalRead(runButton) == HIGH) {
        state = RUN_PRESS;
      }
      break;
    case LEARN_START_PRESS:
      if (digitalRead(learnButton) == LOW) {
        state = LEARNING;
        dot = 0;
        count = 0;
      }
      break;
    case LEARNING:
      hop();
      if (digitalRead(learnButton) == HIGH) {
        state = LEARN_END_PRESS;
      }
      break;
    case LEARN_END_PRESS:
      if (digitalRead(learnButton) == LOW) {
        state = WAITING;
        goal = count;
        count = 0;
        clear_dots();
      }
      break;
    case RUN_PRESS:
      if (digitalRead(runButton) == LOW) {
        state = RUNNING;
        count = 0;
      }
      break;
    case RUNNING:
      bar();
      if (count >= goal) {
        state = WAITING;
        tone(buzzPin, buzzFreq, buzzDuration);
        count = 0;
      }
      break;
  }

  while (millis() < next_tick) 
    ;
  ++count;
}
