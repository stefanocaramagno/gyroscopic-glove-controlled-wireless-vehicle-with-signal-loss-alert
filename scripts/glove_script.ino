// Librerie per il giroscopio
#include <MPU6050_tockn.h>
#include <Wire.h>

// Librerie per la trasmissione
#include <RH_ASK.h>
#include <SPI.h>

// Definizione oggetto giroscopio
MPU6050 mpu6050(Wire);

// Definizione oggetto trasmettitore
RH_ASK transmitter;

// Definizione buffer di trasmissione
char buff[8];

void setup() {
  Serial.begin(9600);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  if (!transmitter.init()) {
    Serial.println("Errore modulo");
    while(1);  
  }

  Serial.println("433-TX");
}

unsigned long t1, dt;
int x,y;

void loop() {
  mpu6050.update();
  dt = millis() - t1;
  if (dt > 220) {
    t1 = millis();

    // Lettura dei valori degli angoli di rotazione del giroscopio (della mano)
    x = mpu6050.getAngleX(); 
    y = mpu6050.getAngleY(); 

    // I valori degli angoli vengono convertiti in una stringa in formato angleX^angleY, utile per la trasmissione
    sprintf(buff, "%d^%d", x, y);
    buff[8] = "\0";

/*
    // Stampa dei valori degli angoli di rotazione, utile nella fase di progettazione per la definizione delle soglie
    Serial.println("=======================================================");  
    Serial.print("angleX : ");Serial.print(mpu6050.getAngleX());
    Serial.print("\tangleY : ");Serial.print(mpu6050.getAngleY());
    Serial.println("=======================================================\n");
*/

    // Trasmissione della stringa contenente le informazioni sugli angoli
    transmitter.send((char *)buff, 8);
    transmitter.waitPacketSent();
  }
}
