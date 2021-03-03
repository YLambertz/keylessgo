  /* Projekt 23 KeylessGO 
 *  Ziel ist es, eine Yamaha XS40012e mithilfe eines Android Gerätes zu starten.
 *  Hierzu wird mittels NFC Sensor der Firma DFrobot eine Ndef Message an den Mikrocontroller NodeMcu V1 (ESP8266) gesendet,
 *  welcher daraufhin mit einem Relaismodul die Zündung freigibt. Der Mikrocontroller bestätigt dies durch eine Ndef Message an das Android Gerät.
 *  Erstellt mithilfe von Open Source Libraries und Beispielcode von https://github.com/Don/NDEF
 *  Dieses Programm wurde erstellt von Yannik Lambertz, Mechatronik Student im 5. Semester an der Hochschule Niederrhein-Fachbereich04
 *  im Rahmen des Bachelorprojekts im Wintersemester 18/19 
 *  
 *  Unterprogramme:
 *  ausschalten():   Ausschalten der Zündung nach einer Sekunde wenn der Killswitch Schalter betätigt wurde
 *  buttonaus():     Abschalten  der Zündung, wenn der Befehl vom Smartphone gesendet wurde
 *  einschalten():   Einschalten der Zündung 
 *  notfallkarte():  EInschalten der Zuendung über die UID einer "Notfallkarte" (NFC TAG)
*/

//Libraries des ESP8266 Wlan Chips 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>                                                                             //Verwendung als Client
#include <ESP8266WebServer.h>                                                                 



                                                                                                     //SoftwareSerial für die UART Übertragung des NFC Sensors (Rx an Pin13, Tx an Pin15)
                                                                                                     //Grund: Bei Verwendung von Serial oder Serial1 des Mikrocontrollers konnten die Daten nicht via USB auf dem Monitor ausgegeben werden, da diese vom USB Port genutzt werden
#include <SoftwareSerial.h>
          SoftwareSerial myserial(13,15);


#include <PN532_SWHSU.h>
                      
                                                                                                    //Libraries für die Komunikation mit dem NFC Sensor (PN532) und die Aufbereitung der Signale
#include "snep.h"                                                                                  
#include "NdefMessage.h"   
#include "NdefRecord.h"

#include <PN532_HSU.h>
#include <PN532.h>
PN532_SWHSU pn532swhsu(myserial);


SNEP nfc(pn532swhsu);




uint8_t ndefBuf[128];                                                                                //Dort wird die NDEF Message zwischengespeichert
uint8_t ndefBuf1[124];                                                                               //Zwischenspeicher für NDEF Message ohne die ersten 4 Bytes 
                                                                                                     
int relais = 12;                                                                                     //Ausgang 12 für die Relais
int turnoff = 14;                                                                                    //Input vom Killswitch des Motorrades                                                                             

    

void setup()                                                                                         //Setup
{

                                                                   


// DEKLARATION EIN/AUSGÄNGE

pinMode(12, OUTPUT);                                                                                //Relais = Digital Out
pinMode(14, INPUT);                                                                                 //Killswitch = Digital Input



//WLAN AUS
WiFi.disconnect();                                                                                  //disconnect, falls mit Wifi verbunden
WiFi.mode(WIFI_STA);                                                                                //Modus = Client

    Serial.begin(115200);                                                                           //Serielle Komunikation auf BAUD 115200
    Serial.println("------- Projekt KeylessGo --------");  
}

void loop()                                                                                         //Schleife (Hauptprogramm)
{

                                                                                                    
    ausschalten(turnoff);                                                                           //falls kein Smartphone in der Halterung ist, aber der Killswitch betätigt wird, soll trotzdem nach 2 sekunden ausgeschaltet werden 

    Serial.println("Warten auf Smartphone Message : ");
 
    notfallkarte();                                                                                 //Aufruf von Notfallkarte()

    
    int msgSize = nfc.read(ndefBuf, sizeof(ndefBuf));                                               //Message per NFC Modul auslesen und in ndefBuf abspeichern
    if (msgSize > 0) {                                                                              // Ausführung, falls Nachrricht vorhanden 
       
        NdefMessage msg  = NdefMessage(ndefBuf, msgSize);  
                                                                                                    //Verarbeitung durch NdefMessage.h Library
   msg.print();                                                                                     //printen von Infos wie payload, payloadLength, Id, IdLength und Type auf dem Seriellen Monitor (115200 BAUD)

   int i=7;
   int j=0;
   for (i=7;i<13;i++)                                                                               //ndefBuf ohne die ersten 4 Bytes in ndefBuf1 speichern (Ersten 4 Bytes enthalten nicht das für uns relevante Passwort)
   {
    ndefBuf1[j] = ndefBuf[i];                                                                       //ndefBuf[7] = ndefBuf1[0] , nefBuf[8] = ndefBuf1[1] usw.  
    j++; 
   }
   int msg1Size = 6;                                                                                //6 stelliges Zahlenpasswort = 720 Möglichkeiten

   
        
  NdefRecord passwort = NdefRecord();                                                             // NdefRecord Library Klassen verwenden
  passwort.setPayload(ndefBuf1, msg1Size);                                                        //payload festlegen in NdefRecord durch übergeben der gelesenen Daten aus ndefBuf1 und festlegen der Länge auf msg1Size= 6 
  String Pw = passwort.getPayload();                                                              //Abholen des Passworts aus getPasswort()als String 

/*
  Serial.print("    Passwort von von NdefRecord erhalten: ");Serial.println(Pw);                  //printen des erhaltenen Passwort-Strings
  Serial.print("    Zählerstand der Fehlversuche: "); Serial.println(Zaehler);                    //printen des Zählerstands der Fehlversuche 
 */


    if( Pw == "123456" )                                                                          //Abgleich des erhaltenen Passworts mit dem an dieser Stelle festgelegten Passwort
    {
    Serial.println("Passwörter stimmen überein");                                                 //printen falls Passwörter übereinstimmen 

    
    einschalten(turnoff);                                                                         // Aufruf von void einschalten() siehe unten
    
    delay(500);                                                                                   //500 ms warten bis die Relais freigeschaltet haben 
    }
        else if ( Pw == "654321" )                                                                //Beispielhaft: Message = Passwort invertiert = Befehl zum Ausschalten
        {
         Serial.println(" Ausschaltbutton gedrückt: Aufruf von void buttonaus() ");
         
         buttonaus();
         }

             else 
            {
              Serial.println("    falsches Passwort");            
         
              
                                                                       
          delay(5000);                                                                            //nach einem falschen Passowort wird 5 sekunden gewartet                                                                
          } 

    }
    else 
    { 
      Serial.println("Leere Message? msgSize = 0"); 
    
    }


}

// END LOOP 


void einschalten( int input )                                                                           //schalten des Relais Ausgangs und Antwort senden
{
  bool Status = digitalRead(input);

      digitalWrite(12, HIGH);                                                                           //Beide Relais mit einem Ausgang HIGH schalten 
      Serial.println("Zündungsrelais via Pin12 EINGESCHALTET");
      


#if 0                                                                                                   //Antwort SENDEN (if0 zu if 1 ändern um zu benutzen )
 
     Serial.println("Antwort Senden");                                                                                      
    NdefMessage message = NdefMessage();                                                                //Definition der Message gemäß NdefMessage Library
    message.addTextRecord("1");                                                                         //Message, welche per Ndef an das Android Gerät gesendet wird
    int messageSize = message.getEncodedSize();

    message.encode(ndefBuf);
      if (0 >= nfc.write(ndefBuf, messageSize)) 
          {                                                                                             //Message per NFC Modul ausgeben 
          Serial.println("Failed Sending");
          }
              else 
              {
              Serial.println("1 gesendet um Zündung zu bestätigen ");
              delay(3000);
              }


#endif

delay(1000);                                                                                            //Nach Einschalten 1 sek warten bevor Loop erneut durchlaufen wird 
}
  
void ausschalten ( int input)                                                                           //Ausschalten des Tachos wenn der Zündunterbrecher gedrückt wurde 
{ 
  int Status;
  Status = digitalRead(input);
      // Serial.print("void ausschalten: Status Killswitch: ");
      // Serial.println(state);
  
    if (Status == HIGH)                                                                         
    { 
     delay(1000);
     digitalWrite(12, LOW);                                                                                //Relais abschalten wenn Killswitch betätigt
     Serial.println("ausschalten() : Killswitch betätigt (Input = 1) -> Zündung ausgeschaltet"); 
    }
        else 
        {
         Serial.println(" ausschalten() : Killswitch I.O. ");
        }
   

}



void notfallkarte(void)                                                                         //lesen einer Mifare Tag Karte und freischalten der Zündung bei richtiger UID der Karte (primitiv und sicherheitstechnisch bedenklich)
{

 Serial.println("Aufruf Notfallkarte");
 PN532 nfc(pn532swhsu);  

 nfc.begin();

 nfc.SAMConfig();   //Rfid Tags lesen 
   
  uint8_t success;
  

  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };                                                      //Buffer um gelesene UID abzuspeichern
  uint8_t uidLength;                                                                            //Laenge UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t notfall[] = {4 ,239 ,84 ,146 ,131 ,92 , 128};                                         //!!!!!!!!!! UID des Notfall Tags !!!!!!!!!
  uint8_t notfall2[] = {4 ,4 ,55 ,146 ,131 ,92 ,129};                                           //UID NotfallTag2 

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    //Printen von Infos über das Tag 
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");


    
    bool equal1 = memcmp (uid, notfall, uidLength);                                              //Vergleich der UID mit der festgelegten Notfallkarte1 durch Funktion memcmp (Memory Compare)
    bool equal2 = memcmp (uid, notfall2, uidLength);                                             //Vergleich der UID mit der festgelegten Nofallkarte2  
    Serial.print("Ergebnis von memcmp: (0 wenn beide gleich): "); 
    Serial.print(equal1); Serial.println(equal2);
    
    if( (equal1 == 0) || (equal2 == 0))                                                          // = 0 wenn beide UIDs gleich sind 
   {
    Serial.println("Nofallkarte erkannt!");
    digitalWrite(12, HIGH); 
    Serial.println("Zuendung durch Notfalldongle eingeschaltet");
 
   }
  
} 
    else
    {
      //nix
    }

}

void buttonaus ( void) 
{
  Serial.println(" Befehl vom Smartphone erhalten, die Zündung abzuschalten -> Zündung AUS "); 
  digitalWrite(12, LOW);  
}
