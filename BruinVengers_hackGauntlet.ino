#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // for oled screen
#include "HackPublisher.h"
#include <dhtnew.h> // for temp sensor

// oled dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHTNEW AM2320(14);

const char *ssid = "ASUS-F8";
const char *password = "K33pi7$@f3%";

HackPublisher publisher("bruinvengers");

// variables and pins for led control
const int channel = 0;
const int freq = 5000;
const int resolution = 10;
const int led_pin = 16;

// pins for electrical components like sensors, leds, and buzzer
const int trigPin = 18;
const int echoPin = 5;
const int redLed = 21;
const int greenLed = 17;
const int buzzer = 19;
const int gasSensorPin = 34;

long duration;
int distance;
int gasSensorValue;
int humidity;
int temperature;

void flashLeds() {
  // turn off green led and flash red leds
  for (int i = 0; i <= 5; i++) {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
    delay(200);
    digitalWrite(redLed, LOW);
    delay(200);
  }
}

void badAirQuality(int gasSensorValue) {
  // update website that air quality causes death
  publisher.store("air_status", "DEATH");
  publisher.send();
  
  tone(buzzer, 2000, 3000); // play buzzer

  // Display warning message to oled
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println("DANGEROUS AIR QUALITY");
  
  // display gas value
  oled.setCursor(0, 20);
  oled.println(gasSensorValue);
  oled.display();

  flashLeds();
  publisher.store("air_status", "");
  publisher.send();
}

void setup() {
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(led_pin, channel);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  Serial.begin(115200);
  while (!Serial) continue;

  // Connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  publisher.begin();

  // check if connected to oled
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
 }

  //print a welcome message to oled
  oled.clearDisplay();
  oled.setCursor(0, 32);
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.println("Welcome BruinVenger");
  oled.setCursor(0, 40);
  oled.println("Good Luck");
  oled.display();
  delay(3000);
}

void loop() {
  // Initialize the distance sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (.0343 * duration) / 2;
  // update website the distance enemy was found
  publisher.store("distance", distance);
  publisher.send();
  
  // Read from gas sensor 
  gasSensorValue = analogRead(gasSensorPin);
  // update to website the alcohol values
  publisher.store("air_quality", gasSensorValue);
  publisher.send();
  delay(500);


  // Read data from temp and humidity sensor
  uint32_t start = micros();
  int am2320_data = AM2320.read();
  uint32_t stop = micros();
  uint32_t duration = stop - start;

  // display to serial monitor and store temp and humid to variables only if new data
  if (duration > 50) {
    humidity = AM2320.getHumidity();
    temperature = AM2320.getTemperature();
    Serial.println("Humidity");
    Serial.print(",\t");
    Serial.println("Temperature");
    Serial.print(",\t");
    publisher.store("humidity", humidity);
    publisher.store("temperature", temperature);
  }

    publisher.store("air_status", "GOOD");
    publisher.send();
    digitalWrite(greenLed, HIGH);

    // display conditions to oled
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.println("Currently Safe");
    oled.setCursor(0, 10);
    oled.println("Distance:");
    oled.setCursor(64, 10);
    oled.println(distance);
    oled.setCursor(0,25);
    oled.println("Gas Val:");
    oled.setCursor(64, 25);
    oled.println(gasSensorValue);
    oled.setCursor(0, 35);
    oled.println("Temp:");
    oled.setCursor(64,35);
    oled.println(temperature);
    oled.setCursor(0, 45);
    oled.println("Humidity");
    oled.setCursor(64,45);
    oled.println(humidity);
    oled.display();

  // when air conditions become dangerous
  if (gasSensorValue >= 2500) {
    badAirQuality(gasSensorValue);
  }

}