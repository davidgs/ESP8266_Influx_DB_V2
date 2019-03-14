/**
    ESP8266 InfluxDbV2: InfluxdbV2.cpp

    Purpose: Helps with sending measurements to an Influx database.

    @author David G. Simmons
    
*/
#include "InfluxDbV2.h"
#include "Arduino.h"

/**
 * Construct an InfluxDbV2 instance.
 * @param host the InfluxDb host
 * @param port the InfluxDb port
 */
InfluxdbV2::InfluxdbV2(String host, uint16_t port) {
  _port = port;
  _host = host;
}

/**
 * Set the Bucket to be used.
 * @param bucket the InfluxDB Bucket which must already exist
 */
void InfluxdbV2::setBucket(String bucket) {
  _bucket = String(bucket);
  begin();
}

/**
 * Set the Organization to be used
 * @param org the Name of the organization unit to use which must already exist
 */
void InfluxdbV2::setOrg(String org){
  _org = String(org);
  begin();
}

/**
 * Set the authorization token
 * @param token the Auth Token from InfluxDBv2 *required*
 */
void InfluxdbV2::setToken(String token){
  _token = String(token);
  begin();
}

void InfluxdbV2::begin() {
  // TODO: recreate HttpClient on db change?
  http.begin(_host, _port, "/api/v2/write?org=" + _org + "&bucket=" + _bucket);
  http.addHeader("Authorization", "Token " + _token);
  http.addHeader("Content-Type", "text/plain");
}

/**
 * Prepare a measurement to be sent.
 */
void InfluxdbV2::prepare(InfluxDataV2 data) { prepared.push_back(data); }

/**
 * Write all prepared measurements into the db.
 */
boolean InfluxdbV2::write() {
  String data = "";
  for (auto const& i : prepared) {
    data = (data == "") ? (i.toString()) : (data + "\n" + i.toString());
  }
  prepared.clear();
  return write(data);
}

/**
 * Write a single measurement into the db.
 */
boolean InfluxdbV2::write(InfluxDataV2 data) { return write(data.toString()); }

/**
 * Send raw data to InfluxDb.
 *
 * @see
 * https://github.com/esp8266/Arduino/blob/cc0bfa04d401810ed3f5d7d01be6e88b9011997f/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h#L44-L55
 * for a list of error codes.
 */
boolean InfluxdbV2::write(String data) {
  if(_token == null || _token.length() < 10){
    Serial.println("#####\nInvalid Access Token\n#####");
    return false;
  }
  Serial.print(" --> writing to host: " + _host + " Port: " + _port + " URL: /api/v2/write?org=" + _org + "&bucket=" + _bucket + ":\n");
  Serial.println(data);

  int httpResponseCode = http.POST(data);
  Serial.print(" <-- Response: ");
  Serial.print(httpResponseCode);

  String response = http.getString();
  Serial.println(" \"" + response + "\"");

  boolean success;
  if (httpResponseCode == 204) {
    success = true;
  } else {
    Serial.println("#####\nPOST FAILED\n#####");
    success = false;
  }

  http.end();
  return success;
}
