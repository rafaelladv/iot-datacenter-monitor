/** Configurações do Blynk */
#include <BlynkSimpleEsp32.h>

#define BLYNK_TEMPLATE_ID "TMPL21Yfp8ZgH"
#define BLYNK_TEMPLATE_NAME "Projeto Extensão  IoT"
#define BLYNK_AUTH_TOKEN "B-80mZJCgvThfPRtzGFO01geLGACeMGy"
char auth[] = BLYNK_AUTH_TOKEN; //Armazena o AuthToken no array auth

BlynkTimer timer;

/** Configurações da Internet */
#include <WiFi.h>
#include <WiFiClient.h>


WiFiServer server(80);
WiFiClient client;

char ssid[] = "brisa-2855866";
char pass[] = "yvurkm2o";
//char ssid[] = "G20"; // Insira o nome da rede Wi-fi utilizada
//char pass[] = "f101redes"; // Insira a senha da rede Wi-fi utilizada
//char ssid[] = "Redmi Note 11 Pro+ 5G";
//char pass[] = "bf5qcenq2u3c5fq";

/** Configurações do CallMeBoot */
#include <HttpClient.h>
#include <Callmebot_ESP32.h>
String apiKey = "8796822";              //Adicione seu número de token que o bot enviou para você no WhatsApp messenger
String phone_number = "+5585997196229"; //Adicione seu número de telefone registrado no aplicativo WhatsApp (o mesmo número que o bot envia para você na url)
String messsage = "Alert!!! Data Center";

/** Configurações do ThingSpeark */
#include <ThingSpeak.h> // Inclui biblioteca ThingsSpeak

#define INTERVALO_ENVIO_THINGSPEAK 20000 /* intervalo entre envios de dados ao ThingSpeak (em ms) */

String chave_escrita_thingspeak = "BI84EA89P6E5E94D"; // Digite sua chave de API de gravação do ThingSpeak
String readAPIKey = "BNR2VEJK7MEWW3BT"; // Digite sua chave de API de gravação do ThingSpeak
unsigned long channelNumber = 2168156; // Digite seu Channel ID do ThingSpeak
char endereco_api_thingspeak[] = "api.thingspeak.com";
unsigned long last_connection_time;

/** Configurações do Sensor DHT11 */
#include <DHT.h> //Biblioteca para sensor DHT11

#define DHTPIN 2 //Pino D2 será responsável pela leitura do DHT11
#define DHTTYPE DHT11 //Define o DHT11 como o sensor a ser utilizado pela biblioteca <DHT.h>
DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)

/** Configurações das Variáveis */
float u = 0.0; //Variável responsável por armazenar a umidade lida pelo DHT11
float t = 0.0; //Variável responsável por armazenar a temperatura lida pelo DHT11
int LedAz = 23;
int LedVm = 22;

/** Função do funcionamento do sensor */
void sensorDHT() {
  
  t = dht.readTemperature(); //Realiza a leitura da temperatura
  u = dht.readHumidity(); //Realiza a leitura da umidade

  if (!isnan(t) && !isnan(u)) {
    Serial.println("\nTemperature: ");
    Serial.println(t); //Imprime na serial a umidade

    Serial.println("\nHumidity: ");
    Serial.println(u); //Imprime na serial a temperatura

    digitalWrite(LedVm, LOW);
    digitalWrite(LedAz, HIGH);
    }else{
    Serial.println("\nSensor desligou \n");
    digitalWrite(LedVm, HIGH);
    delay(1000);
    digitalWrite(LedVm, LOW);
    digitalWrite(LedAz, LOW);
  }

  if (t > 23) {
    Serial.println("\nTemperatura alta!\n");
    digitalWrite(LedAz, LOW);
    digitalWrite(LedVm, HIGH);
    delay(1000);
    digitalWrite(LedVm, LOW);

    // Whatsapp Message
    whatsappMessage(phone_number, apiKey, messsage);
  }

  if (u > 60) {
    Serial.println("\nUmidade alta!\n ");
    digitalWrite(LedAz, LOW);
    digitalWrite(LedVm, HIGH);
    delay(1000);
    digitalWrite(LedVm, LOW);

    // Whatsapp Message
    whatsappMessage(phone_number, apiKey, "Umidade acima do normal!");
  }

  Blynk.virtualWrite(V0, t); //Escreve no pino virtual V6 a temperatura em graus Celsius
  Blynk.virtualWrite(V1, u); //Escreve no pino virtual V0 a umidade em porcentagem

  delay(1000);
}

/** Função que envia os dados para o ThingSpeak */
void envia_informacoes_thingspeak(String string_dados) {
  
    if (client.connect(endereco_api_thingspeak, 80)) {
      
        /* faz a requisição HTTP ao ThingSpeak */
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+chave_escrita_thingspeak+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(string_dados.length());
        client.print("\n\n");
        client.print(string_dados);
         
        last_connection_time = millis();
        Serial.println("\n*** Informações enviadas ao ThingSpeak!");
    }
}


void setup() 
{
  Serial.begin(9600); //Inicializa a comunicação serial
  
  pinMode(LedVm, OUTPUT);
  pinMode(LedAz, OUTPUT);
  
  Blynk.begin(auth, ssid, pass); //Inicializa o Blynk passando como parâmetros o auth token, o nome da rede Wi-Fi e a senha
  
  while (Blynk.connect() == false) {//Verifica se a conexão foi estabelecida
    Serial.print("Sem conexão... n/");
    delay(200);
    Serial.print (".");
    // Whatsapp Message
    whatsappMessage(phone_number, apiKey, "Sem conexão");
    
  }
  Serial.print("Conectado à rede WiFi "); //Imprime a mensagem no monitor serial quando a conexão WiFi for estabelecida
  Serial.println(ssid);

  dht.begin(); //Inicializa o sensor DHT11
  Serial.println("Sensor ligado");
  
  timer.setInterval(2000L, sensorDHT); //Configura a função para ser chamada a cada 2 segundos

  last_connection_time = 0;
  ThingSpeak.begin(client); // Inicializa o ThingSpeak

  // Whatsapp Message
  whatsappMessage(phone_number, apiKey, "ESP32 contectado!");
}


void loop() {
   
  Blynk.run(); //Chama a função Blynk.run
  
  Serial.println("====== Projeto Internet das Coisas ======"); //Imprime a mensagem no monitor serial
  
  sensorDHT(); //Chama a função de leitura do sensor DHT11

  char fields_a_serem_enviados[100] = {0};
     
  // Escreve no canal do ThingSpeak
  if( millis() - last_connection_time > INTERVALO_ENVIO_THINGSPEAK ) {
    t = dht.readTemperature();
    u = dht.readHumidity();
    
    sprintf(fields_a_serem_enviados,"field1=%.2f&field2=%.2f", t, u);
    
    envia_informacoes_thingspeak(fields_a_serem_enviados);
  }

  Serial.println("");
  delay(1000);
}
