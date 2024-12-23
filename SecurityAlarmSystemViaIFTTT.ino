#include "WiFiS3.h"           // Include the WiFiS3 library for internet connectivity
#include <LiquidCrystal_I2C.h> // Include library for LCD

// Define Wi-Fi credentials and IFTTT key here
#define SECRET_SSID "IBM_5000_5G"         
#define SECRET_PASS "Hag0503317925"      
#define WEBHOOKS_KEY "kx6NS_udIzDc_iobddBUUU1ymfbBuueng8rKiOaMeMw" 

// Wi-Fi credentials
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;
WiFiClient client;

// IFTTT server information and event name
char server[] = "maker.ifttt.com";
char event[] = "SecurityWarning";
String webRequestURL = "/trigger/" + String(event) + "/with/key/" + String(WEBHOOKS_KEY);

/* Buzzer */
const int buzPin = A0;

/* LEDs */
const int ledPin1 = 4;
const int ledPin2 = 7;

/* Motion Sensor */
const int motionPin = 2;

/* Button */
const int buttonPin = 3;

/* LCD Screen */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Alert State */
bool alertActive = false;

void setup() {
  Serial.begin(9600);
  pinMode(buzPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(motionPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); 
  lcd.init();
  lcd.backlight();
  Serial.println("System Ready");
  lcd.setCursor(0, 0);
  lcd.print("System Ready");

  // Check WiFi module status
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (1);
  }

  // Attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
  }
  printWifiStatus();
}

void loop() {
  // If motion is detected and alert is not already active
  if (digitalRead(motionPin) == HIGH && !alertActive) {
    alertActive = true;
    triggerAlert();
    triggerIFTTTEvent();  // Send notification
  }

  // If the alert is active, alternate LEDs and check for stop condition
  if (alertActive) {
    alternateLEDs();

    // Check if the button is pressed to stop the alarm
    if (digitalRead(buttonPin) == LOW) {
      stopAlert();
      alertActive = false;
    }
  }
}

void triggerAlert() {
  Serial.println("Motion Detected!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Motion Detected!");
  digitalWrite(ledPin1, HIGH); // Turn on both LEDs initially
  digitalWrite(ledPin2, HIGH);
  digitalWrite(buzPin, HIGH);  // Turn on buzzer
}

void stopAlert() {
  Serial.println("Alert Stopped");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alert Stopped");
  digitalWrite(ledPin1, LOW);  // Turn off both LEDs
  digitalWrite(ledPin2, LOW);
  digitalWrite(buzPin, LOW);   // Turn off buzzer
}

void alternateLEDs() {
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, LOW);
  delay(250);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, HIGH);
  delay(250);
}

void triggerIFTTTEvent() {
  client.stop(); 

  // Attempt to connect to the IFTTT server
  if (client.connect(server, 80)) {  
    Serial.println("connecting...");
    // Construct and send the HTTP GET request
    client.println("GET " + webRequestURL + " HTTP/1.1");
    client.println("Host: maker.ifttt.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();  
  } else {
    Serial.println("connection failed"); 
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
