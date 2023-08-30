#include "Wire.h"                                 //Incluir librería para comunicación I2C
#include "config.h"                               //Incluir configuración del WiFi
#define I2C_DEV_ADDR 0x30                         //Dirección del esclavo

int Ax;                                         //Variable para guardar posición en x
int Ay;                                         //Variable para guardar posición en y
int Distance;                                   //Variable para guardar distancia
uint32_t i = 0;

void onRequest(){                                 //Función para enviar los datos que pide el maestro
  Wire.print(i++);                                //Número de bytes
  Wire.print(" Packets.");
  Serial.println("onRequest");
}

void onReceive(int len){                          //Función para recibir datos
  while(Wire.available()){                        //Ejecutar cada que esté disponible
    Ax = Wire.read();                             //Leer posición en x
    Ay = Wire.read();                             //Leer la posición en y
    Distance = Wire.read();
  }
}

AdafruitIO_Feed *AX = io.feed("ax");              //Indicar los feeds de Adafruit IO
AdafruitIO_Feed *AY = io.feed("ay");
AdafruitIO_Feed *DISTANCE = io.feed("distance");


void setup() {
  Wire.onReceive(onReceive);                      //Definir función para recibir
  Wire.onRequest(onRequest);                      //Definir función para enviar
  Wire.begin((uint8_t)I2C_DEV_ADDR);              //Definir en modo esclavo y su dirección
  Serial.begin(115200);                           //Serial a 115200
  while(! Serial); 

  Serial.print("Connecting to Adafruit IO");
  io.connect();                                   //Función para conectar Adafruit IO

  while(io.status() < AIO_CONNECTED) {            //Esperar conexión
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  io.run();                                       //Mantener Adafruit IO actualizando datos
  Serial.println(Ax);                             //Ver datos en el monitor serial
  Serial.println(Ay);                             //Ver datos en el monitor serial
  Serial.println(Distance);                       //Ver datos en el monitor serial
  
  AX->save(Ax);                                   //Enviar la variable de posicion en x al feed
  AY->save(Ay);                                   //Enviar la variable de posicion en y al feed
  DISTANCE->save(Distance);                       //Enviar la variable de distancia al feed
  delay(5000); //Enviar cada 5s
}