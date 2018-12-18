
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleTimer.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "<SID>";
char pass[] = "<PASS>";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

char mqtt_server[] = "35.246.193.189";
char my_name[] = "blinke_stjerne2";
char topic_subscribe[] = "xmas/command/blinke_stjerne2";
char topic_publish[] = "xmas/status";

SimpleTimer timer;
int timerId;

int LED_CTRL = LED_BUILTIN;
boolean ledActive = false;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_CTRL, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  wifiConnect();
  mqtt();
  setLedActive(true);
}

void loop()
{
  mqtt();
  timer.run();
}

char message_buff[100];
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  for (i = 0; i < length && i < 99; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  String msg = String(message_buff);
    int verdi = msg.toInt();
    timer.deleteTimer(timerId);
    if (verdi >= 10) {
      Serial.println(String(my_name) + ": on");
      setLedActive(true);
    } else if (verdi <= 0) {
      Serial.println(String(my_name) + ": off");
      setLedActive(false);
    } else {
      Serial.println(String(my_name) + ": blink every " + String(200L * verdi) + "ms");
      timerId = timer.setInterval(200L * verdi, ledBlink);
    }
}

void ledBlink()
{
  setLedActive(!ledActive);
}

void setLedActive(boolean active) {
  if (ledActive != active) {
    ledActive = active;
    if (ledActive) {
      digitalWrite(LED_CTRL, LOW);
    } else {
      digitalWrite(LED_CTRL, HIGH);
    }
    publiser(ledActive);
  }
}

void publiser(boolean ledActive) {
  int status = ledActive ? 10 : 0;
  mqttClient.publish(topic_publish, ("{\"" + String(my_name) + "\":" + String(status) + "}").c_str(), true);
}

void wifiConnect() {
  Serial.print("Connecting to Wifi ssid: " + String(ssid));
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(" Connected, ip: ");
  Serial.println(WiFi.localIP());
}

int mqttTimerId;
void mqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection to " + String(mqtt_server) + "...");
    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(mqttCallback);
    if (mqttClient.connect(my_name)) {
      mqttClient.subscribe(topic_subscribe);
      Serial.println(" Connected! Subscribes on topic " + String(topic_subscribe));
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      delay(1000);
    }
  }
  mqttClient.loop();
}
