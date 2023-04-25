// Gabriel Whitlock
// CSC 235 Final Project
// 4-24-23

//////////////////// LIBRARIES ///////////////////////////
// OLED libraries:
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


// Neopixel library:
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
// RTC library:
#include <RTClib.h>
// Humidity library:
#include "DHT.h"
#define DHTTYPE DHT11


//////////////////////// PIN VARS /////////////////////////////////////////

// Sensor Pins:
int moisturePin = A2;
// humidity sensor pin
#define DHTPIN 9

// relay pins
int fanRelay = 13;
int buttonPin = 8;

/////////////////////////////// OTHER VARS /////////////////////////////////////

int state;                          // State of the program
uint8_t WEEKLY_EVENT_START_HH = 7;  // event start time: hour
uint8_t WEEKLY_EVENT_START_MM = 0;  // event start time: minute
uint8_t WEEKLY_EVENT_END_HH = 16;   // event end time: hour
uint8_t WEEKLY_EVENT_END_MM = 0;    // event end time: minute
// Thresshold values:
float humidThreshold = 65;
float tempThreshold = 90;
float moistureThreshold = 500;

// Button 1 vars:
boolean currentState = false;
boolean prevState = false;
boolean isButtonPressed = false;

#define SCREEN_WIDTH 128     // OLED display width,  in pixels
#define SCREEN_HEIGHT 32     // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C  // address for the screen

boolean day = false;
boolean night = false;

//////////////////////// OBJECTS ///////////////////////////////////////

// Neopixel strip vars:
// Strip 1:
#define LED_PIN 4
#define NUM_LEDS 12

// Creates neopixel objects:
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN);     // strip in overhead light


// color of OverHead light
uint32_t colorA = strip.Color(85, 0, 128);

// declare humidity sensor object
DHT dht(DHTPIN, DHTTYPE);
// declaring rtc object:
RTC_DS3231 rtc;

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


///////////////////////////////////// custom logo

// 'hayek', 128x32px custom logo: credit: Joy Han.
const unsigned char epd_bitmap_hayek[] PROGMEM = {
  0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x80, 0x01, 0xe0, 0x00, 0x18, 0x00,
  0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc0, 0x01, 0xe0, 0x00, 0x18, 0x00,
  0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x01, 0xf0, 0x00, 0x38, 0x00,
  0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x00, 0xf0, 0x00, 0x70, 0x00,
  0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xc0, 0x00, 0xf0, 0x00, 0x60, 0x00,
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0xf0, 0x00, 0xe0, 0xff,
  0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xf0, 0x03, 0xff, 0xff,
  0x03, 0x80, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xf0, 0x07, 0xff, 0xff,
  0x03, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe0, 0x00, 0xf0, 0x1f, 0xff, 0xc0,
  0x03, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0xff, 0xe0, 0x00, 0xf0, 0xff, 0xf8, 0x00,
  0x03, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe7, 0xff, 0xe0, 0x00, 0xf1, 0xff, 0x00, 0x00,
  0x03, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0xae, 0xff, 0xe0, 0x00, 0xf7, 0xf8, 0x00, 0x00,
  0x07, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0xae, 0x7f, 0xe0, 0x00, 0xff, 0xe0, 0x00, 0x03,
  0x07, 0x00, 0x28, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xae, 0x7f, 0xe0, 0x00, 0xff, 0x80, 0x00, 0x0f,
  0x07, 0x00, 0x28, 0x03, 0x0c, 0x30, 0x41, 0xfc, 0xae, 0x7f, 0xc0, 0x01, 0xff, 0x00, 0x00, 0x1e,
  0x07, 0x00, 0x2f, 0x00, 0x0c, 0x70, 0xc1, 0xfe, 0xae, 0x7f, 0xc0, 0x01, 0xfc, 0x00, 0x00, 0x38,
  0x07, 0x00, 0x2f, 0xc1, 0xfc, 0x70, 0xc7, 0x04, 0xae, 0x7f, 0xc0, 0x01, 0xf8, 0x00, 0x01, 0xf0,
  0x07, 0x00, 0x28, 0x61, 0xfc, 0x50, 0xc5, 0x04, 0xa0, 0x7f, 0x80, 0x03, 0xf9, 0xff, 0xfb, 0xe0,
  0x03, 0x00, 0x28, 0x63, 0x0c, 0x50, 0xc5, 0x1d, 0xa1, 0xff, 0x80, 0x03, 0xff, 0xff, 0xff, 0xe0,
  0x03, 0x00, 0x28, 0x62, 0x0c, 0x50, 0xc5, 0xf3, 0xa1, 0xff, 0x80, 0x03, 0xff, 0x00, 0x07, 0xff,
  0x03, 0x80, 0x28, 0x62, 0x0c, 0x50, 0xc5, 0x03, 0xa8, 0xff, 0x80, 0x07, 0xf8, 0x00, 0x00, 0x1f,
  0x03, 0x80, 0x28, 0x63, 0x0c, 0x71, 0xc5, 0x1f, 0xac, 0x7f, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00,
  0x03, 0x80, 0x28, 0x63, 0x8c, 0x19, 0xc5, 0x39, 0x8e, 0x7f, 0x00, 0x0f, 0xc0, 0x00, 0x10, 0x00,
  0x03, 0xc0, 0x38, 0x61, 0xf4, 0x0f, 0xc7, 0x73, 0x9e, 0x7f, 0x00, 0x0f, 0x80, 0x00, 0x0f, 0x00,
  0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x87, 0x9e, 0x7e, 0x00, 0x1f, 0x80, 0x03, 0x0f, 0x80,
  0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x07, 0xff, 0xfe, 0x00, 0x1f, 0x00, 0x03, 0x0f, 0xc0,
  0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0xc1, 0xff, 0xff, 0xfe, 0x00, 0x3e, 0x00, 0x01, 0x0f, 0xc0,
  0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xc3, 0xff, 0xff, 0xfc, 0x00, 0x3e, 0x00, 0x00, 0x0f, 0xc0,
  0x00, 0xf0, 0x00, 0x00, 0x00, 0x03, 0x8f, 0xff, 0xff, 0xf8, 0x00, 0x7c, 0x00, 0x00, 0x07, 0xc0,
  0x00, 0xf8, 0x00, 0x00, 0x00, 0x06, 0x1f, 0xff, 0xff, 0xf8, 0x00, 0x7c, 0x00, 0x00, 0x03, 0xc0,
  0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf0, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf0, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00
};



void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  oled.display();
  delay(2000);  // Pause for 2 seconds

  // Clear the buffer
  oled.clearDisplay();
  dht.begin();
  // NeoPixel strip initialization:
  // Strip 1:
  strip.begin();
  strip.clear();
  strip.show();

  // RTC Initialization:
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      ;
  }
  // Automatically sets the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


  
  pinMode(fanRelay, OUTPUT);

  state = 0;
}

void loop() {

  float y = analogRead(moisturePin);
  //////////// BUTTON 1 TOGGLE ///////////////////

  // update the previous reading
  prevState = currentState;
  // update current reading:
  currentState = debounce(buttonPin, prevState);
  isButtonPressed = currentState;

  // toggle if statement
  if (currentState == true && prevState == false) {

    state++;  // change state number by one everytime you push the button
              // Serial.println("it works");
              // oled.display();
    if (state > 4) {
      state = 0;  // resets the mode back to 0 when you run through all other modes.
    }
    // Serial.println(state);
  }

  humid();
  // moisture();
  overHead();


  /////////////////////// Finite State Machine ////////////////////////////////////

  switch (state) {
    case 0:
      oled.clearDisplay();
      oled.drawBitmap(0, 0, epd_bitmap_hayek, 128, 32, 1);
      oled.display();

      break;
      //oled.clearDisplay();
    case 1:
      oled.clearDisplay();
      if (day == true) {
        oled.setTextSize(2);
        oled.setTextColor(WHITE);
        oled.setCursor(0, 0);
        oled.println("The sun is out!");
        oled.display();
        //Serial.println("case 1 day");
      }
      if (night == true) {
        oled.setTextSize(2);
        oled.setTextColor(WHITE);
        oled.setCursor(0, 0);
        oled.println("It's night time!");
        oled.display();
        // Serial.println("case 1 night");
      }
      break;

    case 2:
      oled.clearDisplay();
      if (isnan(dht.readHumidity()) || isnan(dht.readTemperature())) {
        Serial.println("Failed to read from DHT sensor!");
      } else {
        // display temperature
        oled.setTextColor(WHITE);
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.print("Temperature: ");
        oled.setTextSize(2);
        oled.setCursor(0, 10);
        oled.print(dht.readTemperature(true));

        // oled.setCursor(0, 0);
        oled.print(" ");
        oled.setTextSize(2);
        oled.cp437(true);
        oled.write(167);

        oled.setTextSize(2);
        oled.print("F");
        // Serial.println("case 2");
      }

      oled.display();
      break;
    case 3:
      oled.clearDisplay();
      if (isnan(dht.readHumidity()) || isnan(dht.readTemperature())) {
        Serial.println("Failed to read from DHT sensor!");
      } else {
        // display humidity
        oled.setTextColor(WHITE);
        oled.setTextSize(1);
        oled.setCursor(0, 0);
        oled.print("Humidity: ");
        oled.setTextSize(2);
        oled.setCursor(0, 10);
        oled.print(dht.readHumidity());
        oled.print(" %");

        // Serial.println(dht.readHumidity());
      }
      oled.display();
      break;
    case 4:
      oled.clearDisplay();
      
      float val = (y / 1024) * 100;
      // display moisture
      oled.setTextColor(WHITE);
      oled.setTextSize(1);
      oled.setCursor(0, 0);
      oled.print("Moisture: ");
      oled.setTextSize(2);
      oled.setCursor(0, 10);
      oled.print(val);
      oled.print(" %");
      // Serial.println(val);
      oled.display();
      break;
  }
}

/////////////////////////////////////// FUNCTIONS SECTION //////////////////////////////////////////

boolean debounce(int aButtonPin, boolean aLastReading) {
  // get current reading of the button
  boolean currentReading = digitalRead(aButtonPin);

  //compare the current reading to the previous reading
  if (currentReading == true && aLastReading == false) {
    delay(20);
  }
  // return the reading
  return currentReading;
}

// in the case you have a pump to turn on:

// void moisture() {
//   float moistureVal = analogRead(moisturePin);
//   if (moistureVal < moistureThreshold && waterLvl() > waterLvlThreshold) {

//     // pumpMotor->run(FORWARD);
//     digitalWrite(pumpRelay, HIGH);

//   } else {
//     // pumpMotor->run(RELEASE);

//     digitalWrite(pumpRelay, LOW);
//   }
// }

void humid() {
  // read humidity
  float humi = dht.readHumidity();
  //read temperature as Celsius
  float tempC = dht.readTemperature();
  // read temperature as Fahrenheit
  float tempF = dht.readTemperature(true);
  // check if any reads failed
  if (isnan(humi) || isnan(tempC) || isnan(tempF)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {

    if (humi > humidThreshold) {

      digitalWrite(fanRelay, 255);
    } else {

      digitalWrite(fanRelay, LOW);
    }
  }
}

int getAverage(int aPin) {
  // setting up array
  const int numReadings = 100;
  static int readings[numReadings];

  static unsigned long sum = 0;
  static int index = 0;  // remembers the last position

  // read the sensor
  int reading = analogRead(aPin);
  // remove the old reading
  sum -= readings[index];
  // add the new reading
  sum += reading;
  // save the new reading (put it into the array
  readings[index] = reading;
  // update the index
  index++;
  // resets the index after it passes the things in the array
  if (index >= numReadings) {

    index = 0;
  }
  // calculates average and returns it:
  int average = sum / numReadings;

  return average;
}



// uint8_t WEEKLY_EVENT_START_HH = 7;  // event start time: hour
// uint8_t WEEKLY_EVENT_START_MM = 0;  // event start time: minute
// uint8_t WEEKLY_EVENT_END_HH = 16;     // event end time: hour
// uint8_t WEEKLY_EVENT_END_MM = 30;     // event end time: minute

// turns the overhead light on from 7:00 AM to 4:00 PM
void overHead() {
  DateTime now = rtc.now();
  if (now.hour() >= WEEKLY_EVENT_START_HH && now.minute() >= WEEKLY_EVENT_START_MM
      && now.hour() < WEEKLY_EVENT_END_HH && now.minute() >= WEEKLY_EVENT_END_MM) {

    // Serial.println("The sun is shining");
    day = true;
    night = false;
    for (int index = 0; index < strip.numPixels(); index++) {
      strip.setBrightness(50);
      strip.setPixelColor(index, colorA);
      Serial.println("The sun is shining");
    }
    strip.show();

  } else {

    strip.clear();
    day = false;
    night = true;
  }
}
