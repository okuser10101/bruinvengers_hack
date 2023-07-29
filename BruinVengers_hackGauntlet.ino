#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HackPublisher.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char *ssid = "ASUS-F8";
const char *password = "K33pi7$@f3%";

HackPublisher publisher("bruinvengers");


const int trigPin = 18;
const int echoPin = 5;
const int redLed = 21;
const int greenLed = 17;
const int buzzer = 16;

long duration;
int distance;

void enemyFound() {
  publisher.store("distance", distance);
  publisher.send();
  tone(buzzer, 2000, 1000); // play buzzer

  // Display warning message to oled
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println("ENEMY FOUND");
  oled.display();


  // TODO: send that data to the website
  // Flash red leds
  for (int i = 0; i <= 5; i++) {
    digitalWrite(greenLed, LOW);
    digitalWrite(redLed, HIGH);
    delay(500);
    digitalWrite(redLed, LOW);
    delay(500);
  }
}

void setup() {
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
  
 if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println(F("SSD1306 allocation failed"));
  while (true);
 }

  //print a welcome message
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
  // sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  distance = (.0343 * duration) / 2;
  Serial.println(distance);
  delay(200);


  // when status is safe. have a safe funtion or like a if(safe):....
  digitalWrite(greenLed, HIGH);
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.println("Currently Safe");
  oled.display();

  

  // When enemy is found within 50 
  if (distance <= 50) {
    enemyFound();
  }



}
