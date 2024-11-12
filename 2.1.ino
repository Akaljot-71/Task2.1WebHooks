#include <WiFiNINA.h>    // Required for the WiFi connection
#include <ThingSpeak.h>  // Required for establishing communication with ThingSpeak website
#include <DHT.h>         // Required for reading the DHT sensor values
#include "secrets.h"    //Require for using the private credentials 

// Define the digital pin connected to the DHT sensor
#define DHTPIN 7
// Define the type of DHT sensor being used
#define DHTTYPE DHT22

// Initialize the DHT sensor with the specified pin and type
DHT dht(DHTPIN, DHTTYPE);

// Store the SSID and password in character arrays
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// Initialize a WiFi client
WiFiClient client;

// Define the ThingSpeak channel number and write API key
unsigned long channelID = SECRET_CH_ID;
const char* apiKey = SECRET_WRITE_APIKEY;

// Setup function runs once at the beginning
void setup() {
  // Start serial communication
  Serial.begin(9600);
  // Wait for serial connection to be established
  while (!Serial) {
    ;  // Wait for serial port to connect
  }

  // Initialize DHT sensor
  dht.begin();

  // Initialize ThingSpeak communication
  ThingSpeak.begin(client);
}

// Loop function runs repeatedly
void loop() {
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    }
    Serial.println("\nConnected.");
  }
  // Read humidity and temperature from DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if DHT sensor reading is valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print humidity and temperature readings
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Update ThingSpeak with humidity and temperature data
  ThingSpeak.setField(1, humidity);
  ThingSpeak.setField(2, temperature);

  // Send data to ThingSpeak channel and check for success
  int responseCode = ThingSpeak.writeFields(channelID, apiKey);
  if (responseCode == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.print("Problem updating channel. HTTP error code ");
    Serial.println(responseCode);
  }
  delay(60000);
}
