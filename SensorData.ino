#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Temboo.h>
#include <TembooAccount.h> // Contains Temboo account information
#include "DHT.h"

WiFiClient client;

// DHT11 sensor pins
#define DHTPIN 14
#define DHTTYPE DHT11

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

int tweetno = 1;   // Execution count, so this doesn't run forever
int totaltweets = 10;   // Maximum number of times the Choreo should be executed

char ssid[] = "ssid";
char pass[] = "pass";

void setup() {
  Serial.begin(115200);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  // Init DHT 
  dht.begin();

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(ssid, pass);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
}

void loop() {
  
  if (tweetno <= totaltweets) {
    Serial.println("Running StatusesUpdate - Run #" + String(tweetno++));

    // Read data
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    TembooChoreo StatusesUpdateChoreo(client);

    // Invoke the Temboo client
    StatusesUpdateChoreo.begin();

    // Set Temboo account credentials
    StatusesUpdateChoreo.setAccountName(TEMBOO_ACCOUNT);
    StatusesUpdateChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    StatusesUpdateChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String StatusUpdateValue = "#temperature " + String(t);
    StatusesUpdateChoreo.addInput("StatusUpdate", StatusUpdateValue);
    String AccessTokenValue = "yourAccessTokenValue";
    StatusesUpdateChoreo.addInput("AccessToken", AccessTokenValue);
    String ConsumerKeyValue = "yourConsumerKeyValue";
    StatusesUpdateChoreo.addInput("ConsumerKey", ConsumerKeyValue);
    String ConsumerSecretValue = "yourConsumerSecretValue";
    StatusesUpdateChoreo.addInput("ConsumerSecret", ConsumerSecretValue);
    String AccessTokenSecretValue = "yourAccessTokenSecretValue";
    StatusesUpdateChoreo.addInput("AccessTokenSecret", AccessTokenSecretValue);

    // Identify the Choreo to run
    StatusesUpdateChoreo.setChoreo("/Library/Twitter/Tweets/StatusesUpdate");

    // Run the Choreo; when results are available, print them to serial
    StatusesUpdateChoreo.run();
    
    //Second Input
    StatusUpdateValue = "#humidity " + String(h);
    StatusesUpdateChoreo.addInput("StatusUpdate", StatusUpdateValue);

    

    // Run the Choreo; when results are available, print them to serial
    StatusesUpdateChoreo.run();

    while(StatusesUpdateChoreo.available()) {
      char c = StatusesUpdateChoreo.read();
      Serial.print(c);
    }
    StatusesUpdateChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(60000); // wait 60 seconds between StatusesUpdate calls
}
