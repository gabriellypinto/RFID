#include <EEPROM.h>

#define LED 9

void setup(){
   ESP.wdtDisable();
   Serial.begin(9600);
   Serial.println();
   Serial.println();
 
   EEPROM.begin(4096);//Inicia toda a EEPROM.
 
   for (int i = 0; i < 4097; i++)//Loop que irá mostrar no Serial monitor cada valor da EEPROM.
   {
      Serial.print(i);
      Serial.print(": ");
      Serial.println(EEPROM.read(i));
      if(i%100==0){
        ESP.wdtFeed(); 
      }
   }
   Serial.println("PASSEI AQUI 1");
   EEPROM.end();
   Serial.println("PASSEI AQUI 2");
   pinMode(LED, OUTPUT);
   digitalWrite(LED, LOW);
}
void loop(){
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
  ESP.wdtFeed();
}
