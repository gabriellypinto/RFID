#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN    5
#define SS_PIN     4
#define LED        0

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

String list[10] = {""};
bool autorizacao = false;

bool procuraID(String ID);
uint8_t lerCartao(String &cartao);
void confirmaInclusao(String master, String cartao);
void adicionaCartao(String master, String cartao);

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
  String master = " D4 1D 37 A5";
  lerCartao(card);
  if(card != ""){
    adicionaCartao(master, card);
    confirmaInclusao(master, card);
    for (uint8_t i = 0; i < (sizeof(list) / sizeof(list[0])); i++) {
      Serial.print("item: ");
      Serial.print(i);
      Serial.print(" >>");
      Serial.print(list[i]);
      Serial.print("a");
      autorizacao?Serial.println("LIGADO"):Serial.println("DESLIGADO");
    }
  }
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

bool procuraID(String ID) {
  for (uint8_t i = 0; i < (sizeof(list) / sizeof(list[0])); i++) {
    if (list[i] == ID) {
      return true;
    }
  }
  return false;
}

void confirmaInclusao(String master, String cartao) {
  autorizacao = (cartao == master);
}

void adicionaCartao(String master, String cartao) {
  for (uint8_t i = 0; i < (sizeof(list) / sizeof(list[0])); i++) {
    if (list[i] == "" && !procuraID(cartao) && autorizacao && (cartao != master)) {
      list[i] = cartao;
      break;
    }
  }
}
