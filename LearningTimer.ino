/*
  Learning Timer
  
 No fiddling with switches and displays to set how long to wait, just teach the timer 
 by clicking the LEARN button, waiting, and clicking it again. Once you have taught the
 timer, you can run it for that duration any time you like by pressing the RUN button. 
 At the end of the learned time, the buzzer wil sound. Simple.
 
 This code is particularly useful on a system without an accurate clock, as it is
 completely self-calibrating

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

#include <EEPROM.h>

const int learnButton = 8;
const int runButton = 9;
const int buzzPin =  10;

// to avoid unintended very-short durations, times start from when you let go of the button
enum { WAITING, LEARN_START_PRESS, LEARNING, LEARN_END_PRESS, RUN_PRESS, RUNNING } state;

const unsigned int tick = 100; // ms
const unsigned int ticksPerSecond = 1000 / tick;
const unsigned int buzzFreq = 1000;
const unsigned long buzzDuration = 500;

// The LED array is the biggest power draw, so we can save juice by not switching them on all the time
// this uses a relatively crude PWM while waiting
const int ledDutyCyclePercent = 50;

long goal;
long count;
long next_tick;

void ee_write_long(int ee, long value) {
  const byte* p = (const byte*)(const void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
}

long ee_read_long(int ee) {
  long value;
  byte* p = (byte*)(void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return value;
}

void clear_dots() {
  for (int i = 0; i < 8; ++i) {
    digitalWrite(i, HIGH);
  }
}  

// 'hop' a LED along to show learning state
int hop_dot = 0;
void hop() {
  hop_dot = (hop_dot + 1) % 8;
}

// "count down" a bar from all to 0
int bar_dots;
void bar() {
  float progress = count / (goal * 1.0);
  bar_dots = (int)(8 * progress);
}

void setup() {
  for (int i = 0; i < 8; ++i) {
    pinMode(i, OUTPUT);
  }
  clear_dots();
  
  pinMode(learnButton, INPUT);
  pinMode(runButton, INPUT);
  pinMode(buzzPin, OUTPUT);
  
  goal = ee_read_long(0);
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
        hop_dot = 0;
        count = 0;
      }
      break;
    case LEARNING:
      hop();
      if (digitalRead(learnButton) == HIGH) {
        state = LEARN_END_PRESS;
        clear_dots();
      }
      break;
    case LEARN_END_PRESS:
      if (digitalRead(learnButton) == LOW) {
        state = WAITING;
        goal = count;
        ee_write_long(0, goal);
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
        clear_dots();
        state = WAITING;
        tone(buzzPin, buzzFreq, buzzDuration);
        count = 0;
      }
      break;
  }

  long tickle = 0;
  while (millis() < next_tick) {
    int pc = tickle++ % 100;
    int show = pc <= ledDutyCyclePercent ? LOW : HIGH;
    if (state == RUNNING) {
      for (int i = 7; i >= 0; --i) digitalWrite(i, i < bar_dots ? HIGH : show);
    } else if (state == LEARNING) {
      for (int i = 0; i < 8; ++i) digitalWrite(i, i == hop_dot ? show : HIGH);
    }
  }
  
  ++count;
}
