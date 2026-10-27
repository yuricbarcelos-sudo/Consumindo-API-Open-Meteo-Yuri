#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <ArduinoJson.h>
// Credenciais da rede Wi-Fi do laboratório
const char* ssid = "iPhone";
const char* password = "12345678";
WebServer server(80);
// Variáveis globais para armazenar as métricas do clima
float currentTemp = 0.0;
float currentWind = 0.0;
void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Desabilita checagem estrita de certificado SSL
    HTTPClient http;
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    // =========================================================================
    // DESAFIO 1: Insira abaixo a URL completa da API Open-Meteo investigada
    // =========================================================================
    String apiUrl = "https://api.open-meteo.com/v1/forecast?latitude=23.52278&longitude=-46.8368&current_weather=true";
    if (http.begin(client, apiUrl)) {
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
       
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          // ===================================================================
          // DESAFIO 2: Complete com as chaves do JSON para extrair os valores
          // Dica: A temperatura e o vento estão dentro do objeto "current_weather"
          // ===================================================================
         
          currentTemp = doc["current_weather"]["temperature"];
          currentWind = doc["current_weather"]["windspeed"];
          Serial.printf("Temperatura: %.1f C | Vento: %.1f km/h\n", currentTemp, currentWind);
        } else {
          Serial.print("Falha ao ler JSON: ");
          Serial.println(error.c_str());
        }
      }
      http.end();
    }
  }
}
void handleRoot() {
  fetchWeatherData(); // Busca os dados atualizados da API
  String html = "<!DOCTYPE html><html lang='pt-BR'><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Painel Climático - ESP32</title>";
  html += "<style>";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f172a; color: #f8fafc; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; }";
  html += ".card { background: #1e293b; padding: 2rem; border-radius: 12px; text-align: center; max-width: 400px; width: 90%; border: 1px solid #334155; box-shadow: 0 10px 25px rgba(0,0,0,0.3); }";
  html += "h1 { color: #38bdf8; font-size: 1.2rem; text-transform: uppercase; letter-spacing: 1px; margin-bottom: 1.5rem; }";
  html += ".metric { font-size: 2.5rem; font-weight: bold; color: #f1f5f9; margin: 10px 0; }";
  html += ".sub-metric { font-size: 1.1rem; color: #94a3b8; margin-bottom: 20px; }";
  html += ".btn { background: #38bdf8; color: #0f172a; border: none; padding: 12px 24px; font-weight: bold; border-radius: 6px; cursor: pointer; transition: 0.2s; }";
  html += ".btn:hover { background: #0284c7; color: #ffffff; }";
  html += "</style></head><body>";
  html += "<div class='card'>";
  html += "<h1>Clima em Tempo Real (Carapicuíba)</h1>";
 
  // Exibição da temperatura formatada
  html += "<div class='metric'>" + String(currentTemp, 1) + " °C</div>";
 
  // =========================================================================
  // DESAFIO 3: Substitua o texto abaixo para exibir a variável 'currentWind'
  // =========================================================================
  html += "<div class='sub-metric'>Vento: " + String(currentWind, 1) + " km/h</div>";
 
  html += "<button class='btn' onclick='location.reload()'>Atualizar Dados</button>";
  html += "</div></body></html>";
  server.send(200, "text/html", html);
}
void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Conectado!");
  Serial.print("Endereço IP do ESP32: http://");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.begin();
  Serial.println("Servidor HTTP iniciado com sucesso.");
}
void loop() {
  server.handleClient();
}