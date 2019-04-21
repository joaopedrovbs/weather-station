#include "DHT.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_HMC5883_U.h>

#include "_config.h"

#include "system.h"
#include "sensors.h"

// Sensor Objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(1);
Adafruit_BMP085 bmp;

float Sensors::DHT_temp_avg = 0;
float Sensors::DHT_hum_avg = 0;
float Sensors::heading = 0;
float Sensors::pressure = 0;
float Sensors::BMP_temp = 0;
float Sensors::altitude = 0;

uint8_t Sensors::windDirection;
float Sensors::windSpeed;

float DHT_temp = 0;
float DHT_hum = 0;

float DHT_temp_samples[AVG_SAMPLES];
float DHT_hum_samples[AVG_SAMPLES];

long lastWindCheck = 0;
volatile long lastWindIRQ = 0;
volatile byte windClicks = 0;

void read_DHT_run();
void read_BMP085_run();
void read_HMC5883_run();
void read_windSpeed_run();
void read_windDirection_run();
void handleIRQ();
float getWindSpeed();

Thread read_DHT(read_DHT_run, 2000);
Thread read_HMC5883(read_HMC5883_run, 500);
Thread read_BMP085(read_BMP085_run, 200);
Thread read_windDirection(read_windDirection_run, 300);
Thread read_windSpeed(read_windSpeed_run, 1000);

void Sensors::init(){
  pinMode(ANEMOMETER, INPUT);
  pinMode(WIND, INPUT);  
  attachInterrupt(digitalPinToInterrupt(ANEMOMETER), handleIRQ, FALLING);
  
  dht.begin();
  if(!mag.begin()){
    /* There was a problem detecting the HMC5883 ... check your connections */
    LOG("HMC5883 nok");ENDL;
    while(1){
     digitalWrite(LOG_LED, !digitalRead(LOG_LED));
     delay(100); 
    }
  }
  
  LOG("HMC5883 init");ENDL;
  
  if (!bmp.begin()) {
    LOG("BMP085 nok");ENDL;
    while (1) {
      digitalWrite(LOG_LED, !digitalRead(LOG_LED));
      delay(100); 
    }
  }
  LOG("BMP085 init");ENDL;

  CPU.add(&read_DHT);
  CPU.add(&read_HMC5883);
  CPU.add(&read_BMP085);
  CPU.add(&read_windDirection);
  CPU.add(&read_windSpeed);

}


float avg_calc(float * avg, int samples, float value){
  float calc_avg = 0;
  float sum = 0;
  int i = 0;

  for(i=0; i < samples-1; i++){
    avg[i] = avg[i+1];      
  }
  
  avg[samples-1] = value;

  for(i=0; i<samples; i++){
    sum += avg[i];      
  }
  
  calc_avg = sum/(float)samples;
  
  return calc_avg;
}

void read_DHT_run(){

  DHT_temp = dht.readTemperature();
  DHT_hum = dht.readHumidity();

  Sensors::DHT_temp_avg = avg_calc(DHT_temp_samples, AVG_SAMPLES, DHT_temp);
  
  Sensors::DHT_hum_avg = avg_calc(DHT_hum_samples, AVG_SAMPLES, DHT_hum);

  // Check if any reads failed and exit early (to try again).
  if (isnan(DHT_temp) || isnan(DHT_hum)) {
    LOG("Failed to read from DHT sensor!");
    return;
  }
}

void read_HMC5883_run(){
  // Create an 
  sensors_event_t event; 
  mag.getEvent(&event);

  float heading = atan2(event.magnetic.x, event.magnetic.z);
  
  heading += DECLINATION;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  Sensors::heading = heading * 180/M_PI;
}

void read_BMP085_run(){
  Sensors::BMP_temp = bmp.readTemperature();
  
  Sensors::pressure = bmp.readPressure();
  
  Sensors::altitude = bmp.readAltitude(102000);
}



void read_windDirection_run(){
  int windDirectionValue = analogRead(WIND);

  if((windDirectionValue > 750) && (windDirectionValue < 859))
    Sensors::windDirection = 1; //Norte
  else if((windDirectionValue > 480) && (windDirectionValue < 550))
    Sensors::windDirection = 2; //Nordeste
  else if((windDirectionValue > 80) && (windDirectionValue < 120))
    Sensors::windDirection = 3; // Leste
  else if((windDirectionValue > 180) && (windDirectionValue < 240))
    Sensors::windDirection = 4; // Sudeste
  else if((windDirectionValue > 260) && (windDirectionValue < 330))
    Sensors::windDirection = 5; // Sul
  else if((windDirectionValue > 620) && (windDirectionValue < 680))
    Sensors::windDirection = 6; // Sudoeste
  else if((windDirectionValue > 950) && (windDirectionValue < 1020))
    Sensors::windDirection = 7; // Oeste
  else if((windDirectionValue > 860) && (windDirectionValue <= 950))
    Sensors::windDirection = 8; //Noroeste
  else{
    Sensors::windDirection = 0;
  }
}

void read_windSpeed_run(){
  Sensors::windSpeed = getWindSpeed();
  // LOG("Wind: ");LOG(Sensors::windSpeed);ENDL;
}

void handleIRQ(){
	if (millis() - lastWindIRQ > 10) {
		lastWindIRQ = millis(); //Grab the current time
		windClicks++; //There is 1.492MPH for each click per second.
	}
}

//Returns the instataneous wind speed
float getWindSpeed(){
	float deltaTime = millis() - lastWindCheck; //750ms

	deltaTime /= 1000.0; //Covert to seconds

	float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

	windClicks = 0; //Reset and start watching for new wind
	lastWindCheck = millis();

	windSpeed *= 2.4; //4 * 1.492 = 5.968MPH

	return(windSpeed);
}


