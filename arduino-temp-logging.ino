// AUTHOR: Rob Moncur
// PURPOSE: Log temperature data from DHT11 & TMP36

#include <dht.h>
#include <Process.h>

dht DHT;

#define DHT11_PIN 2
#define SENSOR_PIN 0

void setup()
{
  Serial.begin(250000);
  //Serial.begin(9600);
  Serial.println("DHT TEST PROGRAM ");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");

  // Initialize Bridge
  Bridge.begin(250000);

  // Wait until a Serial Monitor is connected.
  int x = 0;
  while (!Serial){
     digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
      x++;
      
      if( x > 60 ) break;
  }
  
  Serial.println("start");

}

void loop()
{
  
  //The "heartbeat" blink. Letting us know its on
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
  Serial.println("DHT TEST PROGRAM ");
  
  // ----- READ TMP36 -----

  //getting the voltage reading from the temperature sensor
  int reading = analogRead(SENSOR_PIN);
  
  // converting that reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0/1024;
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset to degrees ((voltage - 500mV) times 100)
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print("Reading: ");  
  Serial.println(reading);
  Serial.print("Voltage: ");  
  Serial.println(voltage);
  Serial.print("Celsius: ");  
  Serial.println(temperatureC);
  Serial.print("Fahrenheit: ");  
  Serial.println(temperatureF);
  reportValueRM("desk", "TMP36", temperatureC, "C", 0, "PCT");  
  
  // ----- READ DHT11 DATA -----
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time out error,\t");
      break;
    default:
      Serial.print("Unknown error,\t");
      break;
  }

  // DISPLAY DATA
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  float t = DHT.temperature;
  float f = 1.8  * t + 32;
  Serial.println(f, 1);

  //REPORTING THE TEMPERATURE TO THE SERVER
  Serial.println("Start Sending");
  reportValueRM("desk", "DHT11", DHT.temperature, "C", DHT.humidity, "PCT");
  Serial.println("Finish Sending");

  //Blinking the light repeatedly as feedback to the user that the data is being sent
  for (int i = 0; i < 30; i++) {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }

  //Waiting 57 more seconds until the next time we check the temperature
  for (int j = 0; j < 28; j++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(2000);
  }
  
  //Waiting for a second
  delay(1000);
}

//Reporting values to the server
void reportValueRM(String location, String sensor, float temperature, String temperature_unit, float humidity, String humidity_unit) {

  //Building the request url
  String requestUrl = String("http://robmoncur.com/logging/?");
  requestUrl += "location=";
  requestUrl += location;
  requestUrl += "&sensor=";
  requestUrl += sensor;
  requestUrl += "&temperature=";
  requestUrl += temperature;
  requestUrl += "&temperature_unit=";
  requestUrl += temperature_unit;
  requestUrl += "&humidity=";
  requestUrl += humidity;
  requestUrl += "&humidity_unit=";
  requestUrl += humidity_unit;
  Serial.println(requestUrl);

  //Making the request
  Process p;        // Create a process and call it "p"
  p.begin("curl");  // Process that launch the "curl" command
  p.addParameter(requestUrl); // Add the URL parameter to "curl"
  p.run();      // Run the process and wait for its termination

  //Printing the output
  while (p.available() > 0) {
    char c = p.read();
    Serial.print(c);
  }
  // Ensure the last bit of data is sent.
  Serial.flush();
  
}
