#include <FastLED.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ezButton.h>
//------------------------------LED
int const ledSize = 144;
CRGB leds[ledSize];
bool ledState[ledSize];
//------------------------------MPU6050
Adafruit_MPU6050 mpu;
//------------------------------Color
int const colorCount = 6;
int colors[colorCount][3] = {
  { 0, 0, 255 },     //blue
  { 0, 255, 0 },     //red
  { 255, 0, 0 },     //green
  { 0, 255 , 255 },   //purple
  { 255, 255, 0 },   //yellow
  { 255, 255, 255 }  //white
};
int saberRed, saberGreen, saberBlue;
bool lsState = false;
bool flickerState = false;
int colorState = 0;
//------------------------------Button
int isButton1Down;
bool isButton1Clicked;
long button1Start, button1Period;

int isButton2Down;
bool isButton2Clicked;
long button2Start, button2Period;

ezButton button1(5);
ezButton button2(4);
//------------------------------
void setup() {
  Serial.begin(9600);
  //Button
  button1.setDebounceTime(50);
  button2.setDebounceTime(50);
  //FastLED
  FastLED.addLeds<WS2812B, 2>(leds, ledSize);
  FastLED.setBrightness(255);
  FastLED.setMaxRefreshRate(0, false);
  //MPU6050
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}
void loop() {
  //MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  //Button
  button1.loop();
  button2.loop();
  {
    int a = button1.getState();
    Serial.println(a);
    if (isButton1Down != a) {
      button1Start = millis();
      button1Period = 0;
      if (a == LOW) {
      }
      toggleSaber();
    }
    isButton1Down = a;
    if (isButton1Down == HIGH) {
      button1Period = millis() - button1Start;
    } else {
      button1Start = 0;
      button1Period = -1;
    }
  }
  {
    int a = button2.getState();
    if (isButton2Down != a) {
      button2Start = millis();
      button2Period = 0;
      if (a == LOW) {
      }
      changeColor();
    }
    isButton2Down = a;
    if (isButton2Down == HIGH) {
      button2Period = millis() - button2Start;
    } else {
      button2Start = 0;
      button2Period = -1;
    }
  }
  //Saber
  if (lsState) {
    flickSaber(true);
  }
}

void toggleSaber() {
  lsState = !lsState;
  if (lsState) {
    setAll(saberRed, saberGreen, saberBlue, false);
  } else {
    setAll(0, 0, 0, true);
  }
}

void flickSaber(bool show) {
  switch (colorState) {
    case 0:
    case 2:
    case 4:
    int r = saberRed;
    int g = saberGreen;
    int b = saberBlue;
    if (flickerState) {
      if (r < 255)
        r += 20;
      if (g < 255)
        g += 20;
      if (b < 255)
        b += 20;
    }
    int a = random(0, 144);
    int c = random(0, 144);
    if (a > c) {
      int temp = a;
      a = c;
      c = temp;
    }
    for (int i = a; i <= c; i++) {
      if(ledState[i]) {
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
      }
    }
    if(show) {
      FastLED.show();
      delay(random(0, 10));
    }
    flickerState = !flickerState;
    break;
  }
}

void changeColor() {
  colorState++;
  if (colorCount == colorState)
    colorState = 0;
  saberRed = colors[colorState][0];
  saberGreen = colors[colorState][1];
  saberBlue = colors[colorState][2];
  if (lsState) {
    for(int i = 0;i<ledSize;i++){
      ledState[i] = false;
    }
    setAll(saberRed, saberGreen, saberBlue, false);
  }
}


void setAll(int r, int g, int b,bool reverse) {
  bool state = !(r == 0 && b == 0 && g == 0);
    if (reverse) {
      for (int i = ledSize - 1; i >= 0; i--) {
        setPixel(i,r,g,b,state);
      }
    } else {
      for (int i = 0; i < ledSize; i++) {
        setPixel(i,r,g,b,state);
      }
    }
}

void setPixel(int i,int r,int g,int b,bool state){
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
        ledState[i] = !(r == 0 && b == 0 && g == 0);
        flickSaber(false);
        FastLED.show();
}