#include <WiFi.h>
#include <HTTPClient.h>

#define led_verde 2 // Pino utilizado para controle do led verde
#define led_vermelho 40 // Pino utilizado para controle do led vermelho
#define led_amarelo 9 // Pino utilizado para controle do led azul

const int buttonPin = 18;  // Pino utilizado para controle do botão
int buttonState = 0;  // variável para ler o estado do botão
int lastButtonState = LOW; // variável que define último estado do botão como desligado

unsigned long lastDebounceTime = 0; // define último tempo de debounce como 0
unsigned long delayDebounce = 100; // tempo definido para o debounce
int buttonCount = 0; // quantidade de vezes que o botao foi pressionado
unsigned long ultimoPressionado = 0; //estado do botão pela última vez 

const int ldrPin = 4;  // Pino utilizado para o Módulo sensor fotoresistor
int threshold=600; // define limite para o sensor identificar se está claro ou escuro

unsigned long tempo = 0; // define o tempo
unsigned long interval = 0; // define intervalo de tempo


void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(led_amarelo,OUTPUT);
  pinMode(led_verde,OUTPUT);
  pinMode(led_vermelho,OUTPUT);

  // Inicialização das entradas
  pinMode(buttonPin, INPUT); // Inicializa o botão como input (entrada)

  pinMode(ldrPin, INPUT);
  
  // define o estado inicial dos LEDs (desligado)
  digitalWrite(led_amarelo, LOW);
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);

  Serial.begin(9600);
  Serial.print("Conectando-se ao Wi-Fi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Conectado!");

// debounce e checagem de quantas vezes o botão foi pressionado
  unsigned long tempoAtual = millis();
      int buttonStatus = digitalRead(buttonPin);
      if (buttonStatus == LOW) { 
          if (tempoAtual - lastDebounceTime > delayDebounce) {
              lastDebounceTime = tempoAtual;
              ultimoPressionado = tempoAtual;
            }
        }


  // Verifica estado do botão
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

// conexão com o wifi
  if(WiFi.status() == WL_CONNECTED){ // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      }
      http.end();
    }

  else {
    Serial.println("WiFi Disconnected");
  }
}

//função para definir estado do semaforo de noite
void noite() {
  digitalWrite(led_amarelo, HIGH);
  delay(1000);
  digitalWrite(led_amarelo, LOW);
  delay(1000);
}

//função para definir estado do semaforo de dia
void dia() {
  digitalWrite(led_verde, HIGH);
  delay(3000);
  digitalWrite(led_verde, LOW);
  digitalWrite(led_amarelo, HIGH);
  delay(2000);
  digitalWrite(led_amarelo, LOW);
  digitalWrite(led_vermelho, HIGH);
  delay(5000);
  digitalWrite(led_vermelho, LOW);
}

void loop() {
  // cria função para ler informações do Módulo sensor fotoresistor
  int ldrstatus=analogRead(ldrPin);

// imprime mensagem de que esta noite e ativa o modo noite
  if(ldrstatus<=threshold){
    Serial.print("its dark turn on led");
    noite();
    Serial.println(ldrstatus);

//caso esteja dia, led vermelho esteja aceso e botão for pressionado define farol como verde apos 1 segundo
  } else if ((ldrstatus>=threshold) && (led_vermelho == HIGH) && (buttonPin == HIGH)) {
    delay (1000);
    digitalWrite(led_verde, HIGH);
    digitalWrite(led_vermelho, LOW);
    digitalWrite(led_amarelo, LOW);
    
//caso esteja dia, led vermelho esteja aceso e botão for pressionado 3 vezes envia uma requisição HTTP
  } else if ((ldrstatus>=threshold) && (led_vermelho == HIGH) && (buttonCount == 3)) {
    buttonCount = 0;
    HTTPClient http;
    int httpResponseCode = http.GET();
    Serial.print("Requisição enviada.");
    Serial.println(httpResponseCode);
    http.end();

    //se apenas estiver dia, a função dia é ativada
  } else {
    Serial.print("its bright turn off light");
    Serial.println(ldrstatus);
    dia();
  }
}