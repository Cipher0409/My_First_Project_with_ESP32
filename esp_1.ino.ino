/*My first program for esp32
 * Kishan Hegde
 * 06-09-2022
 */
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <wifi_provisioning/manager.h>


//BLE credentials


const char *service_name = "PROV_12345";
const char *pop = "1234567";

//Node Name
char nodeName[] = "ESP32_LED";

//GPIO
static uint8_t gpio_reset = 0;
static uint8_t RED_LED = 23;
static uint8_t YELLOW_LED = 22;
static uint8_t Buzzer = 18;


bool LED1_val = false;
bool LED2_val = false;
bool buzz_val = false;

//device name
static Switch my_button1("RED_LED",&RED_LED);
static Switch my_button2("YELLOW_LED",&YELLOW_LED);
static Switch my_button3("Buzzer",&Buzzer);

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
#else
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
      printQR(service_name, pop, "softap");
#endif
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.printf("\nConnected to Wi-Fi!\n");
      
      delay(500);
      break;
    case ARDUINO_EVENT_PROV_CRED_RECV: {
        Serial.println("\nReceived Wi-Fi credentials");
        Serial.print("\tSSID : ");
        Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
        Serial.print("\tPassword : ");
        Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
        break;
      }
    case ARDUINO_EVENT_PROV_INIT:
      wifi_prov_mgr_disable_auto_stop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("Stopping Provisioning!!!");
      wifi_prov_mgr_stop_provisioning();
      break;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(device_name, "RED_LED") == 0)
  {
    if (strcmp(param_name, "Power") == 0)
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      LED1_val = val.val.b;
      (LED1_val == false) ? digitalWrite(RED_LED, LOW) : digitalWrite(RED_LED, HIGH);
      param->updateAndReport(val);
    }
  }
  //------FOR LED2-------//
  if (strcmp(device_name, "YELLOW_LED") == 0)
  {
    if (strcmp(param_name, "Power") == 0)
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      LED2_val = val.val.b;
      (LED2_val == false) ? digitalWrite(YELLOW_LED, LOW) : digitalWrite(YELLOW_LED, HIGH);
      param->updateAndReport(val);
    }
  }
  if (strcmp(device_name, "Buzzer") == 0)
  {
    if (strcmp(param_name, "Power") == 0)
    {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      buzz_val = val.val.b;
      (buzz_val == false) ? digitalWrite(Buzzer, LOW) : digitalWrite(Buzzer, HIGH);
      param->updateAndReport(val);
    }
  }
}
bool wifi_connected = 0;
void setup() 
{
  pinMode(gpio_reset, INPUT);
  Serial.begin(115200);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(YELLOW_LED, LOW);
  pinMode(YELLOW_LED, OUTPUT);
  digitalWrite(YELLOW_LED, LOW);
  pinMode(Buzzer,OUTPUT);
  digitalWrite(Buzzer,LOW);
  Node my_node;
  my_node = RMaker.initNode(nodeName);
  my_button1.addCb(write_callback);
  my_button2.addCb(write_callback);
  my_button3.addCb(write_callback);
  my_node.addDevice(my_button1);
  my_node.addDevice(my_button2);
  my_node.addDevice(my_button3);
  Serial.printf("\nStarting ESP-RainMaker \n");
  RMaker.start();
  WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32S2
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

}
void loop() 
{
  if (digitalRead(gpio_reset) == LOW) { //Push button pressed
    Serial.printf("Reset Button Pressed!\n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      wifi_connected = 0;
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      wifi_connected = 0;
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    }
  }
  delay(100);
}
