
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <InfluxDbV2.h>

#define INFLUXDB_HOST "192.168.0.32"
#define WIFI_SSID "xxx"
#define WIFI_PASS "xxx"

ESP8266WiFiMulti WiFiMulti;
InfluxdbV2 influx(INFLUXDB_HOST);

void setup() {
  Serial.begin(9600);
  Serial.println(" ### Hello ###");

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WIFI");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  influx.setOrg("default");
  influx.setBucket("telegraf");
  influx.setToken("YourTokenHere");

  Serial.println("Setup done");
}


int loopCount = 0;

void loop() {
  loopCount++;

  InfluxDataV2 row("temperature");
  row.addTag("device", "alpha");
  row.addTag("sensor", "one");
  row.addTag("mode", "pwm");
  row.addValue("loopCount", loopCount);
  row.addValue("value", random(10, 40));

  influx.write(row);

  delay(5000);
}