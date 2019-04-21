#ifndef CONFIG_H
#define CONFIG_H

///////////// Make logging Easier
#define LOG                 Serial.print
#define ENDL                LOG("\n")
#define SERIAL_SPEED        115200

///////////// Wi-Fi Credentials
#define SSID                "Tenda Digital"
#define PASSWORD            "arduinos"

///////////// PinOuts
#define BAT_SENSOR          16
#define WIND                A0
#define ANEMOMETER          14
#define DHTPIN              12     
#define DHTTYPE             DHT11

///////////// System Constants
#define AVG_SAMPLES         5
#define DECLINATION         0.3752

///////////// System outputs
#define LOG_LED             2

#endif