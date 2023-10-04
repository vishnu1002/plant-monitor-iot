
// Plant Monitoring

#include <HTTPClient.h>
#include <WiFi.h>
#include <DHT.h>

//########################################################################################################################################

#define DHTPIN 25
#define SOILPIN 33

//########################################################################################################################################

DHT dht(DHTPIN, DHT22);

const char* ssid = "wifi name";         // SSID
const char* password = "wifi pass";     // Password

const int time_delay = 1000;            // ms [1 hr = 3600000 ms]

//########################################################################################################################################

String scriptUrl = "Google Appscript Script URL";

//########################################################################################################################################

void setup() 
{
  Serial.begin(115200);
  dht.begin();
  wifi_begin();   // Wifi Connection:
}

void loop()
{
  float temp, humid;
  int percent;

  temp_readings(&temp, &humid);
  moist_readings(&percent);

  Serial.println("---------------------------------------------------------");

  google_data_push(temp, humid, percent);   // Google Spreadsheet data push

  delay(time_delay);

}

void temp_readings(float* temp, float* humid)
{
  // DHT22 Sensor Readings:

  float read_temp = dht.readTemperature();
  float read_humid = dht.readHumidity();

  Serial.println("Temperature      : " + (String)read_temp + "C");
  Serial.println("Humidity         : " + (String)read_humid + "%");

  *temp = read_temp;
  *humid = read_humid;
}

void moist_readings(int* percentage )
{
  // Capacitive Soil Moisture Sensor v1.2 Readings:

  // Calibrate the sensor low(wet) and high(dry) values:
  const int wet = 2250; // Low
  const int dry = 4095; // High

  int value = analogRead(SOILPIN);
  int percent = map(value, wet, dry, 100, 0);

  if(percent >= 100)
  {
    percent = 100;
  }

  Serial.println("Moist RAW value  : " + (String)value);
  Serial.println("Moisture percent : " + (String)percent + "%");
  
  if(percent >= 0 && percent < 20)
  {
    Serial.println("Message          : Low Moisture!!");
  }
  else if(percent >= 20 && percent < 70)
  {
    Serial.println("Message          : Good Moisture...");
  }
  else
  {
    Serial.println("Message          : High Moisture!!");
  }

  *percentage = percent;
}

void wifi_begin()
{
  // Wifi Connection:

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("> Connected <");
  Serial.println(WiFi.localIP());
}

void google_data_push(float temp, float humid, int percent)
{
  // Google Spreadsheet Data Push:

  HTTPClient http;
  
  String url = scriptUrl + "?temperature=" + String(temp) + "&humidity=" + String(humid) + "&moisture=" + String(percent);
  Serial.println(url);
  
  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) 
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}



