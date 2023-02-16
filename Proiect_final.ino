#include "BluetoothSerial.h"
#include <ArduinoJson.h> 
#include <WiFi.h> 
#include <WiFiClient.h> 
#include <HTTPClient.h> 

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT; 

const char* ID_WiFi = "Andrei"; 
const char* Parola = "abcd1234"; 

void setup() {
  Serial.begin(115200); 
  SerialBT.begin("Echipa DDD"); 
  Serial.println("Acum poti sa te conectezi Bluetooth");

  Serial.printf("Conectare la %s ", ID_WiFi); 
  WiFi.begin(ID_WiFi, Parola); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");  
  }
  Serial.println("Conectat"); 
}

void loop() {

  if (SerialBT.available()) 
  {

    String tipactivitate = SerialBT.readString(); 

    if (tipactivitate.length() == 23)//se verifica daca actiunea ceruta prin Bluetooth este de a afisa toate caracterele din film;
    {
      WiFiClient client;
      HTTPClient http;
      http.useHTTP10(true);
      http.begin(client, "http://proiectia.bogdanflorea.ro/api/game-of-thrones/characters"); // accesam api-ul cu datele despre toate caracterele;
      http.GET();

      DynamicJsonDocument doc(6144);  

      DeserializationError eroare = deserializeJson(doc, client);
      if (eroare) { // se verifica daca avem ce sa deserializam
        Serial.print("deserializeJson() failed1: ");
        Serial.println(eroare.c_str());
        return;
      }
      for (JsonObject element : doc.as<JsonArray>()) { //se extrag din api informatiile care ne intereseaza si anume ;
                                                       //id-ul,numele complet si imaginea fiecarui caracter cu ajutorul cheii care este unica;
     
        String id = element["id"]; 
        String  fullname = element["fullName"]; 
        String  image = element["imageUrl"];

        SerialBT.println("{\"id\": \"" + id + "\", \"name\": \"" + fullname + "\", \"image\": \"" + image + "\"}");//se afiseaza informatiile extrase din api;
                                                                                                                   //cu ajutorul formatului cerut;
     }
      delay (50); 
    }

    else { //in caz contrar actiunea ceruta prin Bluetooth este de a afisa detaliile despre un caracter anume;


      StaticJsonDocument<96>document;
      DeserializationError eroare = deserializeJson(document, tipactivitate);

      if (eroare) {
        Serial.print("deserializeJson() failed2: ");
        Serial.println(eroare.c_str());
      }

      String action = document["action"]; 
      String id = document["id"]; 


      WiFiClient client; 
      HTTPClient http; 
      http.useHTTP10(true);
      http.begin(client, "http://proiectia.bogdanflorea.ro/api/game-of-thrones/character/"+id);// accesam api-ul cu detaliile despre un caracter anume identificat prin id;
      http.GET();

      StaticJsonDocument<384> document2;

       eroare = deserializeJson(document2, client);

      if (eroare) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(eroare.c_str());
        return;
      }
      String idas = document2["id"]; 
      String fullname = document2["fullName"]; 
      String title = document2["title"]; 
      String family = document2["family"];
      String image = document2["imageUrl"];
         //se extrag din api informatiile care ne intereseaza si anume id-ul,numele complet si imaginea caracterului cu ajutorul cheii care este unica;
      String descriere=fullname+" "+title+" "+family+" "; 
      SerialBT.println("{\"id\": \"" + idas + "\", \"name\": \"" + fullname + "\", \"image\": \"" + image + "\", \"description\": \"" + descriere + "\"}");

      Serial.println(id);
    }
    delay (50);
  }
}
