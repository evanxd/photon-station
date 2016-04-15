#include "application.h"
#include "HttpClient/HttpClient.h"

String SENSOR_ID = "";
String API_KEY = "";
String SERVER_ADDRESS = "api.sensorweb.io";
int SERVER_PORT = 80;

HttpClient http;
http_header_t headers[] = {
  { "Content-Type", "application/x-www-form-urlencoded" },
  { "Accept" , "application/json" },
  { NULL, NULL } // NOTE: Always add the NULL object in the end of the headers.
};
http_request_t request;
http_response_t response;

unsigned int pm10 = 0;
unsigned int pm25 = 0;
unsigned int pm100 = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  int index = 0;
  char value;
  char previousValue;

  while (Serial1.available()) {
    value = Serial1.read();
    if ((index == 0 && value != 0x42) || (index == 1 && value != 0x4d)){
      Serial.println("Cannot find the data header.");
      break;
    }

    if (index == 4 || index == 6 || index == 8 || index == 10 || index == 12 || index == 14) {
      previousValue = value;
    }
    else if (index == 5) {
      pm10 = 256 * previousValue + value;
      Serial.print("{ ");
      Serial.print("\"pm10\": ");
      Serial.print(pm10);
      Serial.print(", ");
    }
    else if (index == 7) {
      pm25 = 256 * previousValue + value;
      Serial.print("\"pm25\": ");
      Serial.print(pm25);
      Serial.print(", ");
    }
    else if (index == 9) {
      pm100 = 256 * previousValue + value;
      Serial.print("\"pm100\": ");
      Serial.print(pm100);
    } else if (index > 15) {
      break;
    }
    index++;
  }
  while(Serial1.available()) Serial1.read();
  Serial.println(" }");

  if (pm10 != 0 && pm25 != 0 && pm100 != 0) {
    Serial.println("Application>\tStart of Loop.");
    request.hostname = SERVER_ADDRESS;
    request.port = SERVER_PORT;
    request.path = "/sensors/" + SENSOR_ID + "/data";
    String pm25String;
    pm25String = pm25;
    request.body = "pm25Index=" + pm25String + "&apiKey=" + API_KEY;

    http.post(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);
    Serial.print("Application>\tHTTP Response Body: ");
    Serial.println(response.body);
  }
  delay(5000);
}
