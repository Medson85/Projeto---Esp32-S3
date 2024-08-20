#include <Arduino.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include "fileHandler.h"

#define led_2 2

Adafruit_NeoPixel led_rgd(1, 48, NEO_GRBW + NEO_KHZ800);
HTTPClient http;
String response;

const char *ssid = "medson";
const char *password = "12345678";
//const char *url = "https://drive.usercontent.google.com/download?id=1JaGn-W9lgkW2hfHmmrop-Nt4MBsl716N&export=download&authuser=0&confirm=t&uuid=6c07dfbb-4584-4c3c-bd58-82c89277fe8e&at=APZUnTWPsPSw96xW9B8LLsZgFHjI:1708982102738";
const char *url = "http://192.168.3.2:3000/download";

bool flag = false;

void statusVivo();
void init();
void testeHttp();
void verificaConexao();
void lerArquivo();


void setup() {

  init();
  led_rgd.setBrightness(200);

}

void loop() {

  statusVivo();
  verificaConexao();
  if( (WiFi.status() == WL_CONNECTED) && (!flag)){
    testeHttp();
    flag = true;
    lerArquivo();
  }

  Serial.println("Tamanho: " + String(response.length()));
  digitalWrite(led_2, LOW);
  delay(5000);

}

void init() {

  //Inicializa a comuncacao serial
  Serial.begin(115200);

  //Inicializa a conexao Wifi
  WiFi.begin(ssid, password);

  //Inicializa URL
  http.begin(url);

  //Inicializa o Led RGB
  led_rgd.begin();

  //Inicializa os arquivos
  if(!SPIFFS.begin(true)){

    Serial.println("SPIFFS Mount Failed");
    return;

  } else{ Serial.println("SPIFFS Sucess"); }

}

void statusVivo() {
  //VERMELHO
  led_rgd.setPixelColor(0, uint32_t (led_rgd.Color(255, 0 , 0)));
  led_rgd.show();
  delay(500);
  
  //VERDE
  led_rgd.setPixelColor(0, uint32_t (led_rgd.Color(0, 255, 0)));
  led_rgd.show();
  delay(500);
  
  //AZUL
  led_rgd.setPixelColor(0, uint32_t (led_rgd.Color(0, 0 , 255)));
  led_rgd.show();
  delay(500);
}

void testeHttp() {
  //const char *url = "https://drive.usercontent.google.com/download?id=1JaGn-W9lgkW2hfHmmrop-Nt4MBsl716N&export=download&authuser=0&confirm=t&uuid=6c07dfbb-4584-4c3c-bd58-82c89277fe8e&at=APZUnTWPsPSw96xW9B8LLsZgFHjI:1708982102738";
  //const char *url = "http://192.168.3.2:3000/download";
  //http.begin(url);

  int httpResponseCode = http.GET();

    if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_PARTIAL_CONTENT) {

        File file = SPIFFS.open("/TesteHex.txt", FILE_WRITE);

        if(!file){
            Serial.println("Erro ao abrir o arquivo.");
            return;
        }
        //salva o corpo da resposta de uma requisição HTTP em um arquivo
        http.writeToStream(&file);
        file.close();

        Serial.println("HTTP Code: " + String(httpResponseCode));

    } else {
        Serial.print("Erro na solicitação. Código de resposta: ");
        Serial.println(httpResponseCode);
    }

    http.end();

}

void verificaConexao() {

  if (WiFi.status() != WL_CONNECTED){
    digitalWrite(led_2, LOW);
  } else {
    digitalWrite(led_2, HIGH);
  }
}

// Lê e imprime o conteúdo do arquivo
void lerArquivo() {

  File file = SPIFFS.open("/TesteHex.txt", FILE_READ);
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para leitura.");
    return;
  }

  while (file.available()) {

    //Serial2.println("mensagem da esp s1: ");
    char c = file.read();

    Serial.print(c);

    //String line = file.readStringUntil('\n'); //lê o conteúdo do arquivo até encontrar um caractere de nova linha (\n)
    //char buffer[line.length() + 1];//soma 1 ao length da linha. Isso é crucial porque as strings em C requerem um caractere finalizador (null terminator, \0) para indicar o final da string. O próprio terminador null não é um caractere imprimível, mas é usado por funções como Serial.print para saber onde a string termina.
    //line.toCharArray(buffer, sizeof(buffer));
    //Serial2.write(buffer, sizeof(buffer) - 1); // exclui o terminador null
  }

  file.close();
}

