// ============================================================
//
//  WIRING:
//    RGB Button   -> Pin 4  (other leg to GND)
//    Smoke Button -> Pin 5  (other leg to GND)
//    Fog machine  -> Pin 2  (OUTPUT)
//    Red cable    -> Pin 9  (PWM ~)
//    Green cable  -> Pin 10 (PWM ~)
//    Blue cable   -> Pin 11 (PWM ~)
// ============================================================

// Pin Definitions 
const int RGB_BUTTON_PIN   = 4;
const int SMOKE_BUTTON_PIN = 5;
const int FOG_PIN          = 2;
const int RED_PIN          = 9;
const int GREEN_PIN        = 10;
const int BLUE_PIN         = 11;

// State 
bool rgbEnabled   = false;
bool smokeEnabled = false;

int lightMode = 0;
int const NUM_COLOURS = 6;

float breathAngle = 0.0;
bool strobeOn = false;
unsigned long lastStrobeTime = 0;
bool policeRed = true;
unsigned long lastPoliceTime = 0;

// Button debounce for RGB button 
int  rgbBtnLast       = HIGH;
int  rgbBtnState      = HIGH;
unsigned long rgbDebounce = 0;

// Button debounce for Smoke button
int  smokeBtnLast       = HIGH;
int  smokeBtnState      = HIGH;
unsigned long smokeDebounce = 0;

const unsigned long DEBOUNCE_DELAY = 50;

// Settings 
const int   CYCLE_SPEED = 2;   // Lower = faster (1–20)
const float BRIGHTNESS  = 1.0; // 0.0 – 1.0

// Colour cycle 
int hue = 0;

void setup() {
  pinMode(RGB_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SMOKE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(FOG_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  digitalWrite(FOG_PIN, LOW);
  allOff();

  Serial.begin(9600);
}

void loop() {
  handleRGBButton();
  handleSmokeButton();

  // Colour
  setLight();

  // Fog                                          
  digitalWrite(FOG_PIN, smokeEnabled ? HIGH : LOW);

  delay(20);
}

void handleRGBButton() {
  int reading = digitalRead(RGB_BUTTON_PIN);

  if (reading != rgbBtnLast) rgbDebounce = millis();

  if ((millis() - rgbDebounce) > DEBOUNCE_DELAY) {
    if (reading != rgbBtnState) {
      rgbBtnState = reading;
      if (rgbBtnState == LOW) {
        lightMode = (lightMode + 1) % NUM_COLOURS; 
      }
    }
  }
  rgbBtnLast = reading;
}


void handleSmokeButton() {
  int reading = digitalRead(SMOKE_BUTTON_PIN);
  if (reading != smokeBtnLast)
  {
    smokeDebounce = millis();
  } 

  if ((millis() - smokeDebounce) > DEBOUNCE_DELAY)
   {
    if (reading != smokeBtnState)
     {
      smokeBtnState = reading;
      if (smokeBtnState == LOW) 
      {
        smokeEnabled = !smokeEnabled;
      }
    }
  }
  smokeBtnLast = reading;
}

void setLight() {

  switch (lightMode) {                                 
    case 0: // Off
      allOff();
      break; 
    case 1: // Red
      setRGB(255, 0, 0);
      break;
    case 2: // Green
      setRGB(0, 255, 0);    
      break;
    case 3: // blue
      setRGB(0, 0, 255);
      break;
    case 4: // Rainbow
      hue = (hue + CYCLE_SPEED) % 360;
      setHue(hue);
      break;
    case 5: // Police
      if (millis() - lastPoliceTime > 500) {
        lastPoliceTime = millis();
        policeRed = !policeRed;
        policeRed ? setRGB(255, 0, 0) : setRGB(0, 0, 255);
      }
      break;
  }
}

void setRGB(int r, int g, int b) {
  analogWrite(RED_PIN,   r * BRIGHTNESS);
  analogWrite(GREEN_PIN, g * BRIGHTNESS);
  analogWrite(BLUE_PIN,  b * BRIGHTNESS);
}

void setHue(int h) {
  int r, g, b;
  int sector    = h / 60;
  int remainder = h % 60;
  int fade      = (remainder * 255) / 60;

  switch (sector) {
    case 0: r = 255;        g = fade;        b = 0;          break; 
    case 1: r = 255 - fade; g = 255;         b = 0;          break;   
    case 2: r = 0;          g = 255;         b = fade;       break; 
    case 3: r = 0;          g = 255 - fade;  b = 255;        break;
    case 4: r = fade;       g = 0;           b = 255;        break;
    case 5: r = 255;        g = 0;           b = 255 - fade; break;
    default: r = 0; g = 0; b = 0;
  }

  r = (int)(r * BRIGHTNESS);
  g = (int)(g * BRIGHTNESS);
  b = (int)(b * BRIGHTNESS);

  analogWrite(RED_PIN,   r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN,  b);
}

void allOff() {
  analogWrite(RED_PIN,   0);
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN,  0);
}
