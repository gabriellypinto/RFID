#include "ESP8266WiFi.h"
#include "PubSubClient.h"

#define DEBUG

#define LED 0   //pino de saida para acionamento da Lampada L1

//informações da rede WIFI
const char* ssid = "LAR";                 //SSID da rede WIFI
const char* password =  "LAR@1480";    //senha da rede wifi

//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "10.6.4.123";   //serve2r
const char* mqttUser = "";              //user
const char* mqttPassword = "";      //password
const int mqttPort = 1883;                     //port
const char* mqttTopicSub = "LED";           //tópico que sera assinado


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  Serial.begin(115200);
  pinMode(LED, OUTPUT);

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

}

void callback(char* topic, byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

#ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("___________________");
#endif

  //aciona saída conforme msg recebida
  if (strMSG == "1") {        //se msg "1"
    digitalWrite(LED, LOW);  //coloca saída em LOW para ligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  } else if (strMSG == "0") {  //se msg "0"
    digitalWrite(LED, HIGH);   //coloca saída em HIGH para desligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }

  if (strMSG == "2") {
  int c = 0;
  while (c < 20) {
      delay(100);
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);
      c++;
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

void loop() {
  if (!client.connected()) {
    reconect();
  }
  client.loop();
}
