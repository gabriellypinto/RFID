#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN    5
#define SS_PIN     4
#define LED        0

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

uint8_t lerCartao(String &cartao);
void adicionaCartao(String lista[]);

void setup() {
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

void loop() {
  String card = "";
  String list[10] = {""};
  lerCartao(card);

}

uint8_t lerCartao(String &cartao) {
  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return 0;
  }
  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return 0;
  }
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cartao.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    cartao.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  cartao.toUpperCase();
}

void adicionaCartao(String lista[], String master) {
  String comp = "";
  while (comp != "") {
    lerCartao(comp);
  }
  if (comp == master) {
    comp = "";
    while (comp != "") {
      lerCartao(comp);
    }
    for (uint8_t i = 0; i < sizeof(lista) / sizeof(lista[0]); i++) {
      if (lista[i] == "") {
        lista[i] = comp;
        break;
      }
    }
  }
}
