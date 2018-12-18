# juleverksted-2018
Felles repo for Juleverksted 2018. Last gjerne opp kode som blir produsert ila. kvelden hit. NB! ta vekk SID og passord fra koden!

## Blinkestjerne
Blinkestjerne er en liten applikasjon som kjører på ESP8266. Applikasjonen kan kontrollere et LED lys til å være av/på eller blinke i et gitt intervall. 

### Biblioteker
Følgende biblioteker må inkluderes får og kunne koble på WiFi og connecte til MQTT broker.
'''
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
'''

### Connect WiFI
Endre SID og wifi-passord. Vi bruker bouvet sitt gjestenett.
'''
char ssid[] = "<SID>";
char pass[] = "<PASS>";
'''

Connect til WiFi kan gjøres med følgende kode.
'''
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
'''

### Connect MQTT
'''
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
'''

### MQTT Publish
'''
void publiser(boolean ledActive) {
  int status = ledActive ? 10 : 0;
  mqttClient.publish(topic_publish, ("{\"" + String(my_name) + "\":" + String(status) + "}").c_str(), true);
}
'''

### MQTT Callback
'''
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
'''
