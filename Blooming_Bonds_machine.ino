//// Token
int sum_points = 0;
int n_value = 0;
int p_value = 0;
int k_value = 0;
bool n_inserted = false;
bool p_inserted = false;
bool k_inserted = false;
int menstrual_level = 0;

void nitrogen_read(int n_read);
void posphorus_read(int p_read);
void potassium_read(int k_read);

//// Moisture sensor
const int AirValue = 866;
const int WaterValue = 332;
int intervals = (AirValue - WaterValue)/5;
int water_level = 0;
void soil_read(int soilMoistureValue);

//// FastLED
#include <FastLED.h>
//// - for menstrual blood level -
#define NUM_LEDS 4 
#define LED_PIN 7
CRGB leds[NUM_LEDS];

//// - for water level -
#define NUM_LEDS_W 4
#define LED_PIN_W 6
CRGB ledsw[NUM_LEDS_W];

//// OLED
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);  

static const unsigned char image_plant_bits[] U8X8_PROGMEM = {0x00,0xc0,0x00,0xf0,0x04,0xf8,0x0e,0xfc,0x1f,0xfc,0x3f,0x7e,0x3f,0x7e,0x3f,0x3e,0x1e,0x0f,0x0c,0x03,0x08,0x01,0x10,0x01,0xa0,0x00,0xc0,0x00,0x80,0x00,0x80,0x00};
static const unsigned char image_FaceNormal_bits[] U8X8_PROGMEM = {0x00,0x00,0x00,0x00,0x3c,0x00,0x80,0x07,0x5e,0x00,0xc0,0x0b,0x7e,0x00,0xc0,0x0f,0x7e,0x00,0xc0,0x0f,0x7e,0x00,0xc0,0x0f,0x3c,0x00,0x80,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x08,0x02,0x00,0x00,0x10,0x01,0x00,0x00,0xe0,0x00,0x00};
static const unsigned char image_FaceNopower_bits[] U8X8_PROGMEM = {0x00,0x00,0x00,0x00,0xc0,0x00,0x60,0x00,0xc0,0x00,0x60,0x00,0xe0,0x00,0xe0,0x00,0x7e,0x00,0xc0,0x0f,0x3e,0x00,0x80,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x03,0x00,0x00,0x0c,0x06,0x00,0x00,0xfe,0x0f,0x00,0x00,0xfe,0x0f,0x00,0x00,0x0e,0x0e,0x00,0x00,0x00,0x00,0x00};
void displayOLED(bool n_inserted, bool p_inserted, bool k_inserted, int menstrual_level, int water_level);

void setup() {
  Serial.begin(9600);

  ////FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812B, LED_PIN_W, GRB>(ledsw, NUM_LEDS_W);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();
  ////OLED
  u8g2.begin();
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setBitmapMode(1);
}

void loop() {
  int soilMoistureValue = analogRead(A0);
  int n_read = analogRead(A1);
  int p_read = analogRead(A2);
  int k_read = analogRead(A3);

  nitrogen_read(n_read);
  posphorus_read(p_read);
  potassium_read(k_read);
  soil_read(soilMoistureValue);

  sum_points = n_value + p_value + k_value;

  FastLED.clear();
  if (n_inserted && p_inserted && k_inserted){
    displayBloodLevel(sum_points);
    displayWaterLevel(water_level);
  }
  displayOLED(n_inserted, p_inserted, k_inserted, menstrual_level, water_level);
  FastLED.show();
  
  

  Serial.println(n_value);
  Serial.println(p_value);
  Serial.println(k_value);
  Serial.println(sum_points);
  Serial.println(soilMoistureValue);
  Serial.println("-----------");
  delay(300);
}

void displayWaterLevel(int soilMoistureValue){
  //// Really wet no more water
  if (water_level == 0){
    for (int i=0; i<NUM_LEDS_W; i++){
      ledsw[i] = CRGB::Black;
      FastLED.show();
    }
  }
  else if (water_level == 1){
    ledsw[0] = CRGB(0,0,255);
    FastLED.show();
  }

  else if (water_level == 2){
    for (int i=0; i< (NUM_LEDS_W-2); i++){
      ledsw[i] = CRGB(0,0,255);
      FastLED.show();
    }
  }

  else if (water_level == 3){
    for (int i=0; i< (NUM_LEDS_W-1); i++){
      ledsw[i] = CRGB(0,0,255);
      FastLED.show();
    }
  }

  else if (water_level == 4){
    for (int i=0; i< NUM_LEDS_W; i++){
      ledsw[i] = CRGB(0,0,255);
      FastLED.show();
    }
  }

  // for (int i=0; i< NUM_LEDS_W; i++){
  //     ledsw[i] = CRGB(0,0,255);
  //     FastLED.show();
  //   }
}


void displayBloodLevel(int sum_points) {
  if (sum_points >= 1 && sum_points <= 2){
    leds[0] = CRGB(255,0,0);
    FastLED.show();
    menstrual_level = 1;
  }
  else if (sum_points >= 3 && sum_points <= 4){
    for (int i=0; i<(NUM_LEDS-2); i++){
      leds[i] = CRGB(255,0,0);
      FastLED.show();
    }
    menstrual_level = 2;
  }
  else if (sum_points >= 5 && sum_points <= 6){
    for (int i=0; i<(NUM_LEDS-1); i++){
      leds[i] = CRGB(255,0,0);
      FastLED.show();
    }
    menstrual_level = 3;
  }
  else if (sum_points >= 7 && sum_points <= 9){
    for (int i=0; i<NUM_LEDS; i++){
      leds[i] = CRGB(255,0,0);
      FastLED.show();
    }
    menstrual_level = 4;
  }
  else{
    for (int i=0; i<NUM_LEDS; i++){
      leds[i] = CRGB::Black;
      FastLED.show();
    }
    menstrual_level = 0;
  }
  
  // for (int i=0; i<NUM_LEDS; i++){
  //     leds[i] = CRGB(255,0,0);
  //     FastLED.show();
  //   }
}

void displayOLED(bool n_inserted, bool p_inserted, bool k_inserted, int menstrual_level, int water_level){
  int status[] = {water_level, menstrual_level};
  u8g2.clearBuffer();
  u8g2.drawXBM(58, 7, 16, 16, image_plant_bits);

  if (n_inserted && p_inserted && k_inserted){
    if (status[0] == 0 && status[1] == 0){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
      u8g2.drawStr(30, 60, "I'm good now");
    }
    else if (status[0] >= 0 && status[0] <= 1 && status[1] != 0){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
      u8g2.drawStr(20, 60, "Fertilize me later");
    }
    else if (status[0] == 1 && status[1] == 0){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
      u8g2.drawStr(20, 60, "Just a sip of water");
    }
    else if (status[0] >= 2 && status[0] <= 4 && status[1] == 0){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNopower_bits);
      u8g2.drawStr(30, 60, "Water please!");
    }
    else if (status[0] == 2 && status[1] == 1){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNopower_bits);
      u8g2.drawStr(5, 60, "Give me a little boost");
    }
    else if (status[0] == 2 && status[1] >= 2 && status[1] <= 4){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
      u8g2.drawStr(20, 60, "Fertilize me later");
    }
    else if (status[0] == 3 && status[1] >= 1 && status[1] <=3){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNopower_bits);
      u8g2.drawStr(8, 60, "Give me a boost shot");
    }
    else if (status[0] == 3 && status[1] == 4){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
      u8g2.drawStr(20, 60, "Fertilize me later");
    }
    else if (status[0] == 4 && status[1] >= 1 && status[1] <= 4){
      u8g2.drawXBM(51, 24, 29, 14, image_FaceNopower_bits);
      u8g2.drawStr(8, 60, "Give me a boost shot");
    }

  }
  else if (n_inserted){
    u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
    u8g2.drawFilledEllipse(45, 50, 6, 6);
    if (p_inserted){
      u8g2.drawFilledEllipse(65, 50, 6, 6);
      if (k_inserted){
        u8g2.drawFilledEllipse(85, 50, 6, 6);
      }
      else{
        u8g2.drawEllipse(85, 50, 6, 6);
      }
    }
    else {
      u8g2.drawEllipse(65, 50, 6, 6);
      if (k_inserted){
        u8g2.drawFilledEllipse(85, 50, 6, 6);
      }
      else{
        u8g2.drawEllipse(85, 50, 6, 6);
      }
    }
  }

  else{
    u8g2.drawXBM(51, 24, 29, 14, image_FaceNormal_bits);
    u8g2.drawStr(40, 60, "Waiting...");
  }
    

  u8g2.sendBuffer();
}

void soil_read(int soilMoistureValue) {
  if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + intervals)){
    water_level = 0;
  }
  
  else if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + 2*intervals)){
    water_level = 1;
  }

  else if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + 3*intervals)){
    water_level = 2;
  }

  else if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + 4*intervals)){
    water_level = 3;
  }

  else if (soilMoistureValue > WaterValue && soilMoistureValue < (WaterValue + 5*intervals)){
    water_level = 4;
  }
}

void nitrogen_read(int n_read) {
  if (n_read >= 0 && n_read <= 45) {
    n_value = 1;
    n_inserted = true;
  }
  else if (n_read >= 160 && n_read <= 190) {
    n_value = 0;
    n_inserted = true;
  }
  else if (n_read >= 315 && n_read <= 355) {
    n_value = 2;
    n_inserted = true;
  }
  else if (n_read >= 850 && n_read <= 900) {
    n_value = 3;
    n_inserted = true;
  }
  else {
    n_value = 0;
    n_inserted = false;
  }
}


void posphorus_read(int p_read){
  if (p_read >= 0 && p_read <= 45) {
    p_value = 1;
    p_inserted = true;
  }
  else if (p_read >= 160 && p_read <= 190) {
    p_value = 0;
    p_inserted = true;
  }
  else if (p_read >= 315 && p_read <= 355) {
    p_value = 2;
    p_inserted = true;
  }
  else if (p_read >= 850 && p_read <= 900) {
    p_value = 3;
    p_inserted = true;
  }
  else {
    p_value = 0;
    p_inserted = false;
  }
}

void potassium_read(int k_read){
  if (k_read >= 0 && k_read <= 45) {
    k_value = 1;
    k_inserted = true;
  }
  else if (k_read >= 160 && k_read <= 190) {
    k_value = 0;
    k_inserted = true;
  }
  else if (k_read >= 315 && k_read <= 355) {
    k_value = 2;
    k_inserted = true;
  }
  else if (k_read >= 850 && k_read <= 900) {
    k_value = 3;
    k_inserted = true;
  }
  else {
    k_value = 0;
    k_inserted = false;
  }
}
