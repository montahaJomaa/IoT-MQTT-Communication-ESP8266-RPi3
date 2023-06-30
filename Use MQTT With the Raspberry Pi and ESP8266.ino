
#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <WiFi.h> // Enables the ESP32 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int ledPin = 2; // This code uses the built-in led for visual feedback that the button has been pressed
const int buttonPin = 0; // Connect your button to pin #13

// WiFi
// WiFi Credentials
const char* ssid = "xxxx";
const char* wifi_password = "xxxxx;

// MQTT
// MQTT Broker Credentials
const char* mqtt_server = "xxxxxx";
const char* mqtt_topic = "test";
// The client id identifies the ESP8266 device.
const char* clientID = "MONTA1";

// Initialise the Pushbutton Bouncer object
Bounce bouncer = Bounce();

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Switch the on-board LED off to start with
  digitalWrite(ledPin, HIGH);

  // Setup pushbutton Bouncer object
  bouncer.attach(buttonPin);
  bouncer.interval(5);

  // Begin Serial on 115200
  Serial.begin(115200);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
  
}

void loop() {
  // Update button state
  // This needs to be called so that the Bouncer object can check if the button has been pressed
  bouncer.update();

  if (bouncer.rose()) {
    // Turn light on when button is pressed down
    // (i.e. if the state of the button rose from 0 to 1 (not pressed to pressed))
    digitalWrite(ledPin, LOW);

    // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
    // Here, "Button pressed!" is the Payload, but this could be changed to a sensor reading, for example.
    if (client.publish(mqtt_topic, "Button 1 pressed!")) {
      Serial.println("Button 1 pushed and message sent!");
    }
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID);
      delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
      client.publish(mqtt_topic, "Button 1 pressed!");
    }
  }
  else if (bouncer.fell()) {
    // Turn light off when button is released
    // i.e. if state goes from high (1) to low (0) (pressed to not pressed)
    digitalWrite(ledPin, HIGH);
  }
}
