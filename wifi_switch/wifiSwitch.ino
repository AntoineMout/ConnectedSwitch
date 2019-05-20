/*****
 
 All the resources for this project:
 https://randomnerdtutorials.com/
 
*****/

// Loading the ESP8266WiFi library and the PubSubClient library
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define gpio13Led 13
#define gpio12Relay 12
#define gpio0Button 0
#define manualDelay 30000 //5 sec

// Change the credentials below, so your ESP8266 connects to your router
bool buttonOn = false;
bool actOnButton = false;
unsigned long timeNow = millis();
unsigned long actOnDelay = 0;
const char* ssid = "SFR-77d8";
const char* password = "DNHTC5MKTQ37";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.0.22";
const char* mqtt_topic = "lampe_1";
const char* mqtt_topic_ack = "lampe_1_ack";
const char* mqtt_topic_state = "lampe_1_state";

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);


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


void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }

  if (messageTemp == "true")
  {
    Serial.print(mqtt_topic); Serial.println(" ON");
    digitalWrite(gpio13Led, LOW);
    digitalWrite(gpio12Relay, HIGH);
    delay(500);
    client.publish(mqtt_topic_ack, "true");
  }
  if (messageTemp == "false")
  {
    Serial.print(mqtt_topic); Serial.println(" OFF");
    digitalWrite(gpio13Led, HIGH);
    digitalWrite(gpio12Relay, LOW);
    delay (500);
    client.publish(mqtt_topic_ack, "false");
  }
  if(messageTemp == "state")
  {
    if (digitalRead(gpio12Relay) == LOW)
    client.publish(mqtt_topic_state, "false" );

    else 
    client.publish(mqtt_topic_state, "true" );
  }

}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) 
    {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe(mqtt_topic);
      delay(500);
      if (digitalRead(gpio12Relay) == LOW)
        client.publish(mqtt_topic_ack, "false" );
  
      else 
        client.publish(mqtt_topic_ack, "true" );

    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void checkTimerRollback()
{
  if (millis()<1000)
  {
    Serial.print("aie...");
    timeNow = millis();
  }
}

void checkButton()
{
  if ((digitalRead(gpio0Button) == LOW) && !actOnButton)
  {
    actOnButton = true;
    if (buttonOn)
    {
      timeNow = millis();
      buttonOn = false; 
    }
    else
    {
      timeNow = millis();
      buttonOn = true;
    }
  }
  if (digitalRead(gpio0Button) == HIGH)
  {
    if (actOnButton)
    {
      if ((millis() - timeNow > 1000) && actOnDelay == 0)
       {
          actOnDelay = manualDelay;
          timeNow = millis();
          Serial.print("manualDelay");
       }
      if (millis() > (timeNow + actOnDelay))
       {
        actOnDelay = 0;
        if (buttonOn)
        {
          Serial.print(timeNow);
          Serial.println(" ON");
          digitalWrite(gpio13Led, LOW);
          digitalWrite(gpio12Relay, HIGH);
          delay(500);
          client.publish(mqtt_topic_ack, "true");
          actOnButton = false;
        }
        else
        {
          Serial.print(timeNow);
          Serial.println(" OFF");
          digitalWrite(gpio13Led, HIGH);
          digitalWrite(gpio12Relay, LOW);
          delay(500);
          client.publish(mqtt_topic_ack, "false");
          actOnButton=false;
        }
       }
    }
    
  }
}

// The setup function sets your ESP GPIOs to Outputs, starts the serial communication at a baud rate of 115200
// Sets your mqtt broker and sets the callback function
// The callback function is what receives messages and actually controls the LEDs
void setup() {
  
  pinMode(gpio13Led, OUTPUT);
  digitalWrite(gpio13Led, HIGH);
  
  pinMode(gpio12Relay, OUTPUT);
  digitalWrite(gpio12Relay, HIGH);

  Serial.begin(115200);
  delay (5000);
  
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(500);
  if (digitalRead(gpio12Relay) == LOW)
     client.publish(mqtt_topic_ack, "false" );
  
  else 
     client.publish(mqtt_topic_ack, "true" );
}

// For this project, you don't need to change anything in the loop function. 
// Basically it ensures that you ESP is connected to your broker
void loop() 
{
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
  {
    client.connect("ESP8266Client");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
  }

  checkButton();
  checkTimerRollback();
}
