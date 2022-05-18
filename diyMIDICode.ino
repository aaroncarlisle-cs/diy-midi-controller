/*
* Credit:
* Original code made by Gustavo Silveira, 2019.
* http://www.musiconerd.com
* http://www.youtube.com/musiconerd
*
* This code was modified by Aaron Carlisle, Arunkarthik Archunan, and Hunter
Fourt,to be used with the Arduino UNO
* This Sketch reads the Arduino's digital and analog ports and send midi notes and
midi control change
*/
#define ATMEGA328 1
#include <MIDI.h> // by Francois Best
MIDI_CREATE_DEFAULT_INSTANCE();

// BUTTONS
const int N_BUTTONS = 9; //* total numbers of buttons
const int BUTTON_ARDUINO_PIN[N_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9, 10}; //* pins of
each button connected straight to the Arduino
int buttonCState[N_BUTTONS] = {}; // stores the button current value
int buttonPState[N_BUTTONS] = {}; // stores the button previous value

// debounce
unsigned long lastDebounceTime[N_BUTTONS] = {0}; // the last time the output pin
was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output
flickers

// POTENTIOMETERS
const int N_POTS = 1; //* total numbers of pots (slide & rotary)
const int POT_ARDUINO_PIN[N_POTS] = {A0}; // pins of each pot connected straight to
the Arduino
int potCState[N_POTS] = {0}; // Current state of the pot
int potPState[N_POTS] = {0}; // Previous state of the pot
int potVar = 0; // Difference between the current and previous state of the pot
int midiCState[N_POTS] = {0}; // Current state of the midi value
int midiPState[N_POTS] = {0}; // Previous state of the midi value
const int TIMEOUT = 300; // Amount of time the potentiometer will be read after it
exceeds the varThreshold
const int varThreshold = 10; // Threshold for the potentiometer signal variation
boolean potMoving = true; // If the potentiometer is moving
unsigned long PTime[N_POTS] = {0}; // Previously stored time
unsigned long timer[N_POTS] = {0}; // Stores the time that has elapsed since the
timer was reset

// MIDI
byte midiCh = 1; // MIDI channel to be used
byte note = 60; // Lowest note to be used
byte cc = 1; // Lowest MIDI CC to be used
void setup() {
 Serial.begin(115200);
 
 // Initialize buttons with pull up resistors
 for (int i = 0; i < N_BUTTONS; i++) {
 pinMode(BUTTON_ARDUINO_PIN[i], INPUT_PULLUP);
 }
}
void loop() {
 buttons();
 potentiometers();
}
void buttons() {
 for (int i = 0; i < N_BUTTONS; i++) {
 buttonCState[i] = digitalRead(BUTTON_ARDUINO_PIN[i]); // read pins from
arduino
 if ((millis() - lastDebounceTime[i]) > debounceDelay) {
 if (buttonPState[i] != buttonCState[i]) {
 lastDebounceTime[i] = millis();
 if (buttonCState[i] == LOW) {
 // Sends the MIDI note ON
 MIDI.sendNoteOn(note + i, 127, midiCh); // note, velocity, channel
 }
 else {
 // Sends the MIDI note OFF
 MIDI.sendNoteOn(note + i, 0, midiCh); // note, velocity, channel
 }
 buttonPState[i] = buttonCState[i];
 }
 }
 }
}

void potentiometers() {
 for (int i = 0; i < N_POTS; i++) { // Loops through all the potentiometers
 potCState[i] = analogRead(POT_ARDUINO_PIN[i]); // reads the pins from arduino
 midiCState[i] = map(potCState[i], 0, 1023, 0, 127); // Maps the reading of the
potCState to a value usable in midi
 potVar = abs(potCState[i] - potPState[i]); // Calculates the absolute value
between the difference between the current and previous state of the pot
 if (potVar > varThreshold) { // Opens the gate if the potentiometer variation
is greater than the threshold
 PTime[i] = millis(); // Stores the previous time
 }
 timer[i] = millis() - PTime[i]; // Resets the timer 11000 - 11000 = 0ms
 if (timer[i] < TIMEOUT) { // If the timer is less than the maximum allowed time
it means that the potentiometer is still moving
 potMoving = true;
 }
 else {
 potMoving = false;
 }
 if (potMoving == true) { // If the potentiometer is still moving, send the
change control
 if (midiPState[i] != midiCState[i]) {
 // Sends the MIDI CC
 MIDI.sendControlChange(cc + i, midiCState[i], midiCh); // cc number, cc
value, midi channel
 potPState[i] = potCState[i]; // Stores the current reading of the
potentiometer to compare with the next
 midiPState[i] = midiCState[i];
 }
 }
 }
}
