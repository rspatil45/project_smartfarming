#include <WiFi.h>
#include <PubSubClient.h>

const int moisturePin = 36;             // moisture sensor pin
const int motorPin = 05;
 

unsigned long interval = 5000;
unsigned long previousMillis = 0;
unsigned long interval1 = 5000;
unsigned long previousMillis1 = 0;

float moisturePercentage;    //moisture reading

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "rspatil";
const char* password = "dAn3Zq66";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "10.42.0.71";

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

char data[80];

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// The setup function sets your DHT sensor, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
 
  Serial.begin(115200);
  delay(10);
  pinMode(motorPin, OUTPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  if (!client.connected()) {
  reconnect();
}
   
}


// For this project, you don't need to change anything in the loop function. 
// Basically it ensures that you ESP is connected to your broker
void loop() {

  if(!client.loop())
     /*
     YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a unique name to the ESP8266.
     Here's how it looks like now:
       client.connect("ESP8266Client");
     If you want more devices connected to the MQTT broker, you can do it like this:
       client.connect("ESPOffice");
     Then, for the other ESP:
       client.connect("ESPGarage");
      That should solve your MQTT multiple connections problem

     THE SECTION IN recionnect() function should match your device name
    */
    client.connect("ESP8266Client");
    

     now = millis();

   unsigned long currentMillis = millis();             // grab current time
     moisturePercentage = ( 100.00 - ( (analogRead(moisturePin) /4095.00) * 100.00 ) );

  if ((unsigned long)(currentMillis - previousMillis1) >= interval1) {
    //Serial.print("Soil Moisture is  = ");
    //Serial.print(moisturePercentage);
    //Serial.println("%");
    previousMillis1 = millis();
  }

  if (moisturePercentage < 50) {
  digitalWrite(motorPin, LOW);         // tun on motor
  }
  
  if (moisturePercentage > 50 && moisturePercentage < 55) {
  digitalWrite(motorPin, LOW);        //turn on motor pump
  }
  
  if (moisturePercentage > 56) {
  digitalWrite(motorPin, HIGH);          // turn off mottor
  }

    static char moist[7];
    dtostrf(moisturePercentage, 6, 2, moist);
    
    String moistReadings = "{\"Moisture\" : \"" + String(moist) + "\"}";
    moistReadings.toCharArray(data, (moistReadings.length() + 1));
    
    // Publishes Temperature and Humidity values
    
    client.publish("moisture", data);
    delay(10000);
    Serial.println(data);
   
  }
