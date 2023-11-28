/*
  Arduino Meteo for relay timed

 Questo sketch permette di caricare via wifi su un database tramite form php i valori
 di temperatura, umidità, pressione atmosferica, livello di luce e carica della batteria
 usando energia solare (se usata).

 Created 01/11/2014
 by Simone Dall'Asta
 
 
 To use the Wifi Shield I used part of the code in 
 the public domain written by Tom Igoe (23 April 2012)
 http://arduino.cc/en/Tutorial/WifiWebClientRepeating
 This code is in the public domain.
 
 
 ------
 
 IMPORTANT: You can use this code quoting me and my website and other credits, thanks. Have fun :)
 http://www.dallaf.it/simo/meteo_simo.php 
 
 ------
 */
 
 



#include <SPI.h>
#include <WiFi.h>

/*
This is the library of Adafruit for reading temperature and humidity.
Download and info:
http://www.adafruit.com/ 
*/
#include "DHT.h"

/*
This is the library of Sparkfun Electronics for reading temperature and atmospheric pressure.
Download and info:
https://www.sparkfun.com/
*/
#include <SFE_BMP180.h>


#include <Wire.h>



SFE_BMP180 pressure; //Creo istanza pressione
#define DHTPIN 2 //Pin per rilevare la temperatura
#define DHTTYPE DHT22   /*Tipo di DHT = DHT 22  (AM2302) (esiste anche il DHT11 
                        ma con range ridotto di misurazione) */
 
#define PIN_LIGHT 6


const int analogInPin = A0; //Pin per rilevare la carica della batteria
int BatteryValue = 0; //Inizializzo a zero la carica della batteria
DHT dht(DHTPIN, DHTTYPE); //Creo istanza temperatura del tipo DHTTYPE


char ssid[] = "your_network"; //Il nome della tua rete
char pass[] = "your_password"; //Password della rete wifi
int status = WL_IDLE_STATUS; //Inizializzo a "inattivo"
WiFiClient client; //Istanza client
char status_p;
double T,P,p0,a;
int errore_pressione = 0; //Per vedere se ci sono errori nell'inizializzazione del sensore BMP180. Impongo flag a zero.
char server[] = "www.tuosito.it"; //Inserisci dominio del tuo sito dove hai caricato il file php (solo il dominio!)

String temp_title = "temperatura=";
String umid_title = "umidita=";
String press_title = "pressione=";
String lux_title = "luce=";
String batt_title = "battery=";
String yourdata[3];
String temp, pressi, umid, lux, batt; //Dati da inviare con il Wifi
int inLux = A2; //Pin per rilevare la luce

void setup() {
  Serial.begin(9600);

  Serial.println("INIZIO"); //Per debug
  delay(1000);
  pinMode(PIN_LIGHT, OUTPUT);
}

void loop() {
  digitalWrite(PIN_LIGHT, LOW); //Per adesso non lo uso (risparmio un po')
  int tentativi = 0; /*Conto i tentativi di connessione (max 10), evidentemente
                      la rete non è al momento disponibile*/
  
  // Tento di collegarmi alla rete
  while ( status != WL_CONNECTED) {
    tentativi += 1; //Ogni volta incremento di uno il contatore
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000); //Attendo 10 secondi per la connessione
    if (tentativi >= 10) while(1); /*Se per 10 volte non si è connesso
                                  allora manca la rete e attendo in un
                                  ciclo infinito che il relay stacchi
                                  la corrente ad Arduino e riprovo al
                                  ciclo successivo.*/
  }
  // Sei ora connesso e stampo nome, ip e potenza segnale
  printWifiStatus();




/*
Created by Mike Grusin, SparkFun Electronics 10/24/2013
Modified by Simone Dall'Asta 01/11/2014
*/
  Serial.println(" ");
  if (pressure.begin())
    Serial.println("BMP180 init success"); //Inizializzato con successo il sensore BMP180
  else
  {
    Serial.println("BMP180 init fail\n\n");
    errore_pressione = 1; // errore pressione. Flag = 1.
  }

  Serial.println(" ");
  if (errore_pressione == 0) //Se il sensore è stato inizializzato correttamente...
  {
    status_p = pressure.startTemperature(); //Inizia a misurare la temperatura con sensore BMP180
    if (status_p != 0)
    {
      // Wait for the measurement to complete:
      delay(status_p);
      // Retrieve the completed temperature measurement:
      // Note that the measurement is stored in the variable T.
      // Function returns 1 if successful, 0 if failure.
      status_p = pressure.getTemperature(T);
      if (status_p != 0)
      {
        // Print out the measurement:
        Serial.print("temperature: ");
        Serial.print(T,2); //Scrivi la temperatura (con due cifre dopo la virgola) in gradi Celsius
        Serial.print(" deg C, ");
        Serial.print((9.0/5.0)*T+32.0,2); //e in gradi Fahrenheit, con due cifre dopo la virgola
        Serial.println(" deg F");
        status_p = pressure.startPressure(3); //Inizia a misurare la pressione con sensore BMP180
        if (status_p != 0)
        {
          // Wait for the measurement to complete:
          delay(status_p);


          status_p = pressure.getPressure(P,T);
          if (status_p != 0)
          {
            // Print out the measurement:
            Serial.print("absolute pressure: ");
            Serial.print(P,2); //Scrivi pressione in mbar
            Serial.print(" mb, ");
            Serial.print(P*0.0295333727,2);
            Serial.println(" inHg"); //e in inHg

          }
          else Serial.println("error retrieving pressure measurement\n");
        }
        else Serial.println("error starting pressure measurement\n");
      }
      else Serial.println("error retrieving temperature measurement\n");
    }
    else Serial.println("error starting temperature measurement\n");
  }
  
  /*Rilevazione temperatura e umidità
  
  Created by ladyada 
  Modified by Simone Dall'Asta (01/11/2014)
  */
  float temperatura; /*Temperatura che verrà inviata (scelgo la minore tra i due valori misurati 
                    DHT22 e BMP180) ma si potrebbe anche fare una media.*/
  delay(3000); //Attenzione: per il DHT22 frequenza di campionamento non superiore a 0,5 Hz (una volta ogni 2 secondi)
  float temp_dht22 = (float)dht.readTemperature(); //Leggo il valore della temperatura in gradi C dall'istanza dht
  delay(3000);
  float umi_dht22 = (float)dht.readHumidity(); //Leggo il valore dell'umidità dall'istanza dht
  delay(3000);
  
  /*
  Scelgo la temperatura minore tra quella del DHT22 e BMP180 per diminuire l'errore dovuto a riscaldamento del sensore.
  Questo solo se il sensore di pressione era stato inizializzato correttamente.
  */
  if ((float)temp_dht22 > 0 && errore_pressione == 0)
  {
    if ((float)temp_dht22 < T)
    {
      temperatura = (float)temp_dht22;
    }
    else
    {
      temperatura = (float)T;
    }
  }
  else 
  {
    temperatura = (float)temp_dht22;
  }
  
  delay(2000);
  digitalWrite(PIN_LIGHT, HIGH); //Pongo ad HIGH il pin per la luce (ora mi serve)
  delay(2000);
  int lux_value = analogRead(inLux); //Leggi valore di luce in una scala da 0 a 1023
  
  /*se si vuole calcolare direttamente in percentuale il livello della luce
  decommentare la riga seguente. Io lo faccio al momento del recuper dei dati per
  poter correggere eventuali errori dovuti a luci impreviste (es. lampioni che si accendono)
  senza dover ricaricare ogni volta lo sketch.
  */
  //int lux_perc = map(lux_value, 0, 1023, 0, 100); 
  Serial.print("Luminosita': ");
  Serial.println(lux_value);
  delay(2000);
  digitalWrite(PIN_LIGHT, LOW); //Metti a LOW il pin della luce. Ora non serve più
  delay(2000);
  BatteryValue = analogRead(analogInPin); //Leggi il valore della carica della batteria
  delay(2000);


/*Prepara ora i dati da inviare al form php che li caricherà sul database.
Il dato sarà del tipo (ad esempio per la temperatura):
temperatura = 12.49
*/
  temp = temp_title + (float)temperatura;
  umid = umid_title + (float)umi_dht22;
  pressi = press_title + (float)P;
  lux = lux_title + lux_value;
  batt = batt_title + BatteryValue;
  

  inviaDato(); //Invia i dati
  
  /*Disconnetti dalla rete e attendi che il relay stacchi la corrente*/
  status = WiFi.disconnect();
  //Delay 10 secondi dopo disconnessione
  delay(10000);
  Serial.print("Stato connessione: "); //Per debug. Dovrebbe essere 1
  Serial.println(status);
  status = WL_IDLE_STATUS;
  Serial.print("Stato connessione: "); //Per debug. Dovrebbe essere 0
  Serial.println(status);


  while(1); //Attesa infinita
}



void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}




// Questo metodo tenta di connettersi al server e poi prova ad inviare i dati uno per volta
void inviaDato() {

  
  
  //Invia temperatura
  
  //prova a connetterti al server
  if (client.connect(server, 80)) {
    Serial.println("Connesso...");
    
    client.println("POST /inserisci.php HTTP/1.1"); /*Inserisci l'URL dove hai inserito il file php (senza dominio, 
                                                    ad es /prova/inserisci.php se il file si trova in 
                                                    www.tuosito.it/prova/inserisci.php
                                                    */
    
    client.println("Host: www.tuosito.it"); //Metti il dominio dove si trova inserisci.php (SOLO dominio!)
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(temp.length()); //Lunghezza dato
    Serial.println(temp); //Per debug
    Serial.println(temp.length()); //Per debug
    client.println();
    client.println(temp);
  } 
  else {
    //Se fallisce la connessione
    Serial.println("Connection failed");
    Serial.println("Disconnecting.");
    client.stop();
  }
  //Attendi 10 secondi e disconnetti il client
  delay(10000);
  client.stop();
  Serial.println("Automatically disconnected after 10 seconds.");
  Serial.println("Continua...");
  
  
/*stessa cosa per gli altri dati*/


//Invia umidità
  if (client.connect(server, 80)) {
    Serial.println("Connesso...");

    client.println("POST /inserisci.php HTTP/1.1"); /*Inserisci l'URL dove hai inserito il file php (senza dominio, 
                                                    ad es /prova/inserisci.php se il file si trova in 
                                                    www.tuosito.it/prova/inserisci.php
                                                    */
    
    client.println("Host: www.tuosito.it"); //Metti il dominio dove si trova inserisci.php (SOLO dominio!)
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(umid.length());
    Serial.println(umid);
    Serial.println(umid.length());
    client.println();
    client.println(umid);
  } 
  else {
    //Se fallisce.
    Serial.println("Connection failed");
    Serial.println("Disconnecting.");
    client.stop();
  }
  //Disconnetti client dopo 10 secondi
  delay(10000);
  client.stop();
  Serial.println("Automatically disconnected after 10 seconds.");
  Serial.println("Continua...");
  
  
  
  //Invia batteria
  if (client.connect(server, 80)) {
    Serial.println("Connesso...");

    client.println("POST /inserisci.php HTTP/1.1"); /*Inserisci l'URL dove hai inserito il file php (senza dominio, 
                                                    ad es /prova/inserisci.php se il file si trova in 
                                                    www.tuosito.it/prova/inserisci.php
                                                    */
    
    client.println("Host: www.tuosito.it"); //Metti il dominio dove si trova inserisci.php (SOLO dominio!)
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(batt.length());
    Serial.println(batt);
    Serial.println(batt.length());
    client.println();
    client.println(batt);
  } 
  else {
    //Se fallisce.
    Serial.println("Connection failed");
    Serial.println("Disconnecting.");
    client.stop();
  }
  //disconnetti client dopo 10 secondi
  delay(10000);
  client.stop();
  Serial.println("Automatically disconnected after 10 seconds.");
  Serial.println("Continua...");
  
  
  //ora invio la pressione ma SOLO se non ci sono stati errori con il sensore BMP180
  if (errore_pressione == 0)
  {
    if (client.connect(server, 80)) {
      Serial.println("Connessione...");

      client.println("POST /inserisci.php HTTP/1.1"); /*Inserisci l'URL dove hai inserito il file php (senza dominio, 
                                                    ad es /prova/inserisci.php se il file si trova in 
                                                    www.tuosito.it/prova/inserisci.php
                                                    */
    
    client.println("Host: www.tuosito.it"); //Metti il dominio dove si trova inserisci.php (SOLO dominio!)
      client.println("User-Agent: Arduino/1.0");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded;");
      client.print("Content-Length: ");
      client.println(pressi.length());
      Serial.println(pressi);
      Serial.println(pressi.length());
      client.println();
      client.println(pressi);
    } 
    else {
      //Se fallisce.
      Serial.println("Connection failed");
      Serial.println("Disconnecting.");
      client.stop();
    }
    //Disconnetti client dopo 10 secondi.
    delay(10000);
    client.stop();
    Serial.println("Automatically disconnected after 10 seconds.");
    Serial.println("Continua...");
  }
  
  
  //Invia livello luce
  if (client.connect(server, 80)) {
    Serial.println("Connesso...");

    client.println("POST /inserisci.php HTTP/1.1"); /*Inserisci l'URL dove hai inserito il file php (senza dominio, 
                                                    ad es /prova/inserisci.php se il file si trova in 
                                                    www.tuosito.it/prova/inserisci.php
                                                    */
    
    client.println("Host: www.tuosito.it"); //Metti il dominio dove si trova inserisci.php (SOLO dominio!)
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(lux.length());
    Serial.println(lux);
    Serial.println(lux.length());
    client.println();
    client.println(lux);
  } 
  else {
    //Se fallisce.
    Serial.println("Connection failed");
    Serial.println("Disconnecting.");
    client.stop();
  }
  //Disconnetti client dopo 10 secondi.
  delay(10000);
  client.stop();
  Serial.println("Automatically disconnected after 10 seconds.");
  Serial.println("Continua...");
}




