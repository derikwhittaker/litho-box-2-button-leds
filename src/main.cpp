/*
  Push Button 
    1 wire to neurtral
    1 wire to D2

  LED
    1 wire to neurtral
    1 wire to power
    1 wire to D3

*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 2
#define COLOR_BUTTON_PIN 6
#define POWER_BUTTON_PIN 4

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 32

// Colors
// 1 White (255, 255, 255)
// 2 Red (255, 0, 0)
// 3 Blue (255, 0, 255)
// 4 Yellow (255, 230, 0)
// 5 Green (0, 128, 0)

int red[] = {255, 255, 72, 255, 0};
int green[] = {255, 0, 0, 230, 128};
int blue[] = {255, 0, 255, 0, 0};

int active_color = 0;
const int max_colors = 5;

enum LedStates
{
  ON,
  OFF
};

enum SwitchStates
{
  IS_OPEN,
  IS_RISING,
  IS_CLOSED,
  IS_FALLING
};

LedStates ledState = ON;
SwitchStates ledSwitchState = IS_OPEN;
SwitchStates powerSwitchState = IS_OPEN;

int prior_power_button_state_was_pressed = false;
int power_is_off = false;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Some functions of our own for creating animated effects -----------------

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color)
{
  Serial.println(F("Running ColorWipe...."));
  for (int i = 0; i < strip.numPixels(); i++)
  {                                // For each pixel in strip...
    strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
  }

  strip.show(); //  Update strip to match
}

void cycleColor()
{

  // start over
  if (active_color < max_colors)
  {
    active_color++;
  }
  else
  {
    active_color = 0;
  }

  Serial.println(F("Button Pressed, changing color to "));

  int l_red = red[active_color];
  int l_green = green[active_color];
  int l_blue = blue[active_color];

  colorWipe(strip.Color(l_red, l_green, l_blue));
}

void cyclePower()
{
  if (!power_is_off)
  {
    // turn off
    colorWipe(strip.Color(0, 0, 0));
  }
  else
  {
    int l_red = red[active_color];
    int l_green = green[active_color];
    int l_blue = blue[active_color];

    colorWipe(strip.Color(l_red, l_green, l_blue));
  }

  power_is_off = !power_is_off;

  Serial.println(F("Button Pressed, Cycling Power.. "));
}

void colorPinStateMachine()
{
  byte pinIs = digitalRead(COLOR_BUTTON_PIN);

  switch (ledSwitchState)
  {
  case IS_OPEN:
  {
    if (pinIs == HIGH)
    {
      ledSwitchState = IS_RISING;

      Serial.println(F("IS_OPEN - Is High... is rising"));
    }
    break;
  }
  case IS_RISING:
  {
    ledSwitchState = IS_CLOSED;
    Serial.println(F("IS_RISING"));
    break;
  }
  case IS_CLOSED:
  {
    if (pinIs == LOW)
    {
      ledSwitchState = IS_FALLING;
      Serial.println(F("IS_CLOSED - Is Low... is falling"));
    }
    break;
  }
  case IS_FALLING:
  {
    cycleColor();
    ledSwitchState = IS_OPEN;
    Serial.println(F("IS_FALLING"));
    break;
  }
  }
}

void powerPinStateMachine()
{
  byte pinIs = digitalRead(POWER_BUTTON_PIN);

  switch (powerSwitchState)
  {
  case IS_OPEN:
  {
    if (pinIs == HIGH)
    {
      powerSwitchState = IS_RISING;

      Serial.println(F("POWER IS_OPEN - Is High... is rising"));
    }
    break;
  }
  case IS_RISING:
  {
    powerSwitchState = IS_CLOSED;
    Serial.println(F("POWER IS_RISING"));
    break;
  }
  case IS_CLOSED:
  {
    if (pinIs == LOW)
    {
      powerSwitchState = IS_FALLING;
      Serial.println(F("POWER IS_CLOSED - Is Low... is falling"));
    }
    break;
  }
  case IS_FALLING:
  {
    cyclePower();
    powerSwitchState = IS_OPEN;
    Serial.println(F("POWER IS_FALLING"));
    break;
  }
  }
}

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println(F("LED Tester is starting up..."));

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(COLOR_BUTTON_PIN, INPUT_PULLUP);
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);

  colorWipe(strip.Color(red[active_color], green[active_color], blue[active_color]));
}

void loop()
{
  powerPinStateMachine();
  colorPinStateMachine();
}
