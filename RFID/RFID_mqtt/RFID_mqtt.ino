#include "SPI.h"
#include "MFRC522.h"
#include "ESP8266WiFi.h"
#include "PubSubClient.h"


#define RST_PIN    5 //GPIO05 D1
#define SS_PIN     4 //GPIO04 D2
#define LED        0 //GPIO00 D3

#define DEBUG

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

//informações da rede WIFI
const char* ssid = "LAR";                 //SSID da rede WIFI
const char* password =  "LAR@1480";    //senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "10.6.4.123";   //server
const char* mqttUser = "";              //user
const char* mqttPassword = "";      //password
const int mqttPort = 1883;                     //port
const char* mqttTopicSub = "RFID";           //tópico que sera assinado

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);   // Inicia a serial
  SPI.begin();      // Inicia  SPI bus
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef DEBUG
    Serial.println("Conectando ao WiFi...\n");
#endif
  }
#ifdef DEBUG
  Serial.println("Conectado na rede WiFi\n");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address
#endif

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
#ifdef DEBUG
    Serial.println();
    Serial.println("Conectando ao Broker MQTT...\n");
#endif

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
#ifdef DEBUG
      Serial.println("Conectado\n");
#endif

    } else {
#ifdef DEBUG
      Serial.print("falha estado: ");
      Serial.print(client.state());
      Serial.println();
#endif
      delay(2000);

    }
  }
  //subscreve no tópico
  client.subscribe(mqttTopicSub);

  mfrc522.PCD_Init();   // Inicia MFRC522

  Serial.println("Selecione uma opção: ");
  Serial.println();
  Serial.println("1 - Leitura de cartão");
  Serial.println("2 - Cadastro de cartão");
  Serial.println("3 - Definir master");
  Serial.println();
  digitalWrite(LED, HIGH);
  pinMode(LED, OUTPUT);

}

void callback(char* topic, byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

#ifdef DEBUG
  Serial.println();
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("___________________");
  Serial.println();
#endif

  //aciona saída conforme msg recebida
  if (strMSG == "1") {
    Serial.print("Leitura de dados do cartao: ");
    // Procura por cartao RFID
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Seleciona o cartao RFID
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Mostra UID na serial
    Serial.print("UID da tag :");
    String conteudo = "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    conteudo.toUpperCase();

    if (conteudo.substring(1) == "01 D1 FC 52" || "03 1C A0 F6") //UID 1 - Cartao
    {
      Serial.println("Liberado !");
      Serial.println();
      digitalWrite(LED, LOW);     // LIGA LED OU/ ativa rele, abre trava solenoide
      delay(3000);              // DELAY /espera 3 segundos
      digitalWrite(LED, HIGH);  // DESlIGA LED OU /desativa rele, fecha  trava solenoide
    }
  } else if (strMSG == "0") {   // Procura por cartao RFID
    Serial.print("Cadastro do cartao: ");
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Seleciona o cartao RFID
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Mostra UID na serial
    Serial.print("UID da tag :");
    String conteudo = "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    conteudo.toUpperCase();

    if (conteudo.substring(1) == "01 D1 FC 52" || "03 1C A0 F6") //UID 1 - Cartao
    {
      Serial.println("master definido !");
      Serial.println();
      digitalWrite(LED, LOW);     // LIGA LED OU/ ativa rele, abre trava solenoide
      delay(3000);              // DELAY /espera 3 segundos
      digitalWrite(LED, HIGH);  // DESlIGA LED OU /desativa rele, fecha  trava solenoide
    }
  }
  if (strMSG == "2") {
    // Procura por cartao RFID
    Serial.print("Cadastro do master: ");
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Seleciona o cartao RFID
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Mostra UID na serial
    Serial.print("UID da tag :");
    String conteudo = "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    conteudo.toUpperCase();

    if (conteudo.substring(1) == "01 D1 FC 52" || "03 1C A0 F6") //UID 1 - Cartao
    {
      Serial.println("CADASTRADO !");
      Serial.println();
      digitalWrite(LED, LOW);     // LIGA LED OU/ ativa rele, abre trava solenoide
      delay(3000);              // DELAY /espera 3 segundos
      digitalWrite(LED, HIGH);  // DESlIGA LED OU /desativa rele, fecha  trava solenoide
    }
  }
}

//função pra reconectar ao servido MQTT
void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
#ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT.\n");
#endif

    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");

    if (conectado) {
#ifdef DEBUG
      Serial.println("Conectado!");
#endif
      //subscreve no tópico
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
#ifdef DEBUG
      Serial.println("Falha durante a conexão. Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
#endif
      //Aguarda 10 segundos
      delay(10000);
    }
  }
}

void loop()
{
  if (!client.connected()) {
    reconect();
  }
  client.loop();
  // Procura por cartao RFID
  while (!mfrc522.PICC_IsNewCardPresent()) {
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Seleciona o cartao RFID
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }
    //Mostra UID na serial
    Serial.print("UID da tag :");
    String conteudo = "";
    byte letra;
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    Serial.print("Mensagem : ");
    conteudo.toUpperCase();

    if (conteudo.substring(1) == "01 D1 FC 52" || "03 1C A0 F6") //UID 1 - Cartao
    {
      digitalWrite(LED, LOW);     // LIGA LED OU/ ativa rele, abre trava solenoide
      delay(3000);              // DELAY /espera 3 segundos
      digitalWrite(LED, HIGH);  // DESlIGA LED OU /desativa rele, fecha  trava solenoide
    }
  }

}
