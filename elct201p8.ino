// Sound-reactive LED meter, 5 LEDs (digital sound sensor module)
//
// Wiring:
//   Sensor VCC -> 5V, GND -> GND, DO (digital out) -> D2
//   LEDs -> D8, D9, D10, D11, D12 (each through a 470 ohm to 1k resistor to GND)

const int SENSOR_PIN = 2;
const int LED_PINS[] = {8, 9, 10, 11, 12};
const int NUM_LEDS = 5;

const unsigned long WINDOW_MS   = 1500;  // how long events are remembered
const unsigned long DEBOUNCE_MS = 50;    // ignore re-triggers within this time

// Many modules (KY-038, LM393 boards) pull DO HIGH on sound.
// If yours is inverted, change this to LOW.
const int SOUND_LEVEL = HIGH;

unsigned long eventTimes[NUM_LEDS];      // timestamps of the most recent events
unsigned long lastEvent = 0;
bool lastState = !SOUND_LEVEL;

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
    eventTimes[i] = 0;
  }
}

void loop() {
  unsigned long now = millis();
  bool state = (digitalRead(SENSOR_PIN) == SOUND_LEVEL);

  // Register a new event on a rising edge, with debounce
  if (state && !lastState && (now - lastEvent) > DEBOUNCE_MS) {
    lastEvent = now;
    // Shift older events down, store newest at index 0
    for (int i = NUM_LEDS - 1; i > 0; i--) {
      eventTimes[i] = eventTimes[i - 1];
    }
    eventTimes[0] = now;
  }
  lastState = state;

  // Count events still inside the time window
  int level = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (eventTimes[i] != 0 && (now - eventTimes[i]) <= WINDOW_MS) {
      level++;
    }
  }

  // Light that many LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], i < level ? HIGH : LOW);
  }
}