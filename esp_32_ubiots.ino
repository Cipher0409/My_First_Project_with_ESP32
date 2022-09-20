


#include <WiFi.h>
#include <PubSubClient.h>


#define WIFISSID "YOUR PASSWORD" // Put your WifiSSID here
#define PASSWORD "YOUR WIFI PASSWORD" // Put your wifi password here
#define TOKEN "YOUR UBIDOTS TOKEN" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "UNIQUE 8-12 ASCII NAME" 

#define VARIABLE_LABEL "led_1"
#define VARIABLE_LABEL_SUBSCRIBE "led_2"
#define DEVICE_LABEL "esp32" // Assig the device label

#define led_1 26
#define led_2 27 


char mqttBroker[]  = "things.ubidots.com";
char payload[100];
char topic[150];
char topicSubscribe[100];
// Space to store values to send

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) 
    {
      Serial.println("Connected");
      client.subscribe(topicSubscribe);
    } 
    else 
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  
  String message(p);
  if (message == "0") 
  {
    //Serial.println("BOTH  LED's are OFF");
    digitalWrite(led_1,LOW);
    digitalWrite(led_2,LOW);
    
  }
  if(message == "1")
  {
     digitalWrite(led_1, HIGH);
     digitalWrite(led_2,HIGH);
  }
  
 
  Serial.write(payload, length);
  Serial.println();
}

void setup() 
{
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);

  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL);
  sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL_SUBSCRIBE);
  
  client.subscribe(topicSubscribe);
}



void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    client.subscribe(topicSubscribe);   
    reconnect();
  }

  /*if (led_1==1 && led_2==1)
  {
    digitalWrite(led_1, HIGH);
    digitalWrite(led_2, HIGH);
  }
  else
  {
    digitalWrite(led_1, LOW);
    digitalWrite(led_2, LOW);
  }*/
  
 
  

  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Cleans the payload
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_SUBSCRIBE);
  
  
  //Serial.println("Publishing data to Ubidots Cloud");
  client.publish(topic, payload);
  client.loop();
  delay(1000);
}
