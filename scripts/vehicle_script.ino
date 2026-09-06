// File con le note musicali
#include "musical_notes.h"

// Librerie per il ricevitore radio a 433MHz
#include <RH_ASK.h>
#include <SPI.h> 

// Definizione dei pin per il controllo dei motori
#define LEFT1 2
#define LEFT2 3

#define RIGHT1 4
#define RIGHT2 5

// Definizione del pin del buzzer
#define BUZZER 6

// Definizione oggetto per il ricevitore Rx
RH_ASK receiver;

// Definizione buffer di ricezione
uint8_t buff[8];
uint8_t buflen = 8;

void setup() {
  Serial.begin(9600);

  pinMode(LEFT1, OUTPUT);
  pinMode(LEFT2, OUTPUT);
  pinMode(RIGHT1, OUTPUT);
  pinMode(RIGHT2, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  if (!receiver.init())
    Serial.println("init failed");
  Serial.println("rx ok");
}

int stato = 0;
int x, y;
int segnaleAssente = 0;
int velocita;

void loop() {
  switch(stato) {
    case 0: // Ricezione segnale radio
      if(receiver.recv(buff, &buflen)) {

        buff[8] = "\0";

        // Conversione della stringa formattata in due interi
        if (sscanf(buff, "%d^%d", &x, &y) == 2) {

          //Stampa i numeri estratti
          /*
          Serial.print("Primo numero: ");
          Serial.print(x);
          Serial.print("\tSecondo numero: ");
          Serial.println(y);
          */

          stato = 1;

          segnaleAssente = 0;

        } else 

          //Se il formato è errato
          Serial.println("Formato non valido. Riprova.");
        

      } else {  // se la macchina non riceve il segnale per 40 letture consecutive emana il suono che indica l'assenza di segnale
        segnaleAssente++;

        if (segnaleAssente > 40) stato = 4;
        else stato = 0;

        delay(20);
      }

    break;

    case 1: // Movimento macchina - permette di decidere se la macchina deve andare aventi/indietro o deve ruotare
      if(y > -20 && y < 20) stato = 2;
      else stato = 3;
      
    break;

    case 2: // Movimento avanti/indietro
      
      if(x > -100 && x < -10){  // La macchina deve andare avanti

        digitalWrite(LEFT1, HIGH);
        digitalWrite(LEFT2, LOW);

        digitalWrite(RIGHT1, HIGH);
        digitalWrite(RIGHT2, LOW);

      } else if(x > 8 && x < 100) {  // La macchina deve andare indietro

        digitalWrite(LEFT1, LOW);
        digitalWrite(LEFT2, HIGH);

        digitalWrite(RIGHT1, LOW);
        digitalWrite(RIGHT2, HIGH);

      } else {  // La macchina deve stare ferma

        digitalWrite(LEFT1, LOW);
        digitalWrite(LEFT2, LOW);

        digitalWrite(RIGHT1, LOW);
        digitalWrite(RIGHT2, LOW);

      }

      stato = 0;
      delay(10);

    break;

    case 3: // Movimento destra/sinistra

      if(y > -120 && y < -20){   // La macchina deve andare a sinistra
      
        digitalWrite(LEFT1, HIGH);
        digitalWrite(LEFT2, LOW);

        digitalWrite(RIGHT1, LOW);
        digitalWrite(RIGHT2, HIGH);

      } else if(y > 20 && y < 120) {   // La macchina deve andare a destra

        digitalWrite(LEFT1, LOW);
        digitalWrite(LEFT2, HIGH);

        digitalWrite(RIGHT1, HIGH);
        digitalWrite(RIGHT2, LOW);
      } 

      stato = 0;

      delay(10);
    break;

    case 4: // Nessuna ricezione, musica
      digitalWrite(LEFT1, LOW);
      digitalWrite(LEFT2, LOW);

      digitalWrite(RIGHT1, LOW);
      digitalWrite(RIGHT2, LOW);

      if(posizione >= sizeMelody) posizione = 0;
      
      // Calcola la durata della nota in millisecondi.
      noteDuration = 1000 / timeJingleBells[posizione];

      tone(BUZZER, melodyJingleBells[posizione], noteDuration);
  
      // Per distingure le note viene settato una pausa tra una nota e l'altra
      // Questo intervallo è pari al 30% della durata.      
      pauseBetweenNotes = noteDuration * 1.30;

      delay(pauseBetweenNotes);

      posizione++;

      stato = 0;
    break;
  }
}
