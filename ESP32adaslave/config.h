/************************ Adafruit IO Config *******************************/

#define IO_USERNAME "SCHadrian19"
#define IO_KEY "aio_wWIc54XkhliWV7kTXS6QSa7i6oQ2"

/******************************* WIFI **************************************/

#define WIFI_SSID "CUCV"
#define WIFI_PASS ""

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
