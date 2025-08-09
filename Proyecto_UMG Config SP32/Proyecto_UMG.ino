#include <WiFi.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <HTTPClient.h>
#include <DHTesp.h>

// Configuración del sensor
#define DHTPIN 4 // es donde esta colocado fisicamente al SP32
DHTesp dht;

// Datos de  red Wi-Fi (hotspot del Celular)
const char* ssid = "SP32config";
const char* password = "UMGconfigSP32";

const char* serverIPv4 = "10.117.231.66"; //ip asignada a la laptop para envio API
const int   serverPort = 8000; // puerto para API 


void setup() {
  Serial.begin(115200); //activa el monitor
  dht.setup(DHTPIN, DHTesp::DHT11);  // inicia el sensor

  // Conectar a Wi-Fi
  WiFi.begin(ssid, password); // conecta al wifi
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("."); // imprime puntos en lo que conecta
  }

  Serial.println("\nConectado a Wi-Fi");
  Serial.print("Dirección IPv4: ");
  Serial.println(WiFi.localIP()); //muestra la ip asignado para el ESP32

}

void loop() {
  float temperatura = dht.getTemperature(); // lee el sensor
  float humedad = dht.getHumidity();

  if (!isnan(temperatura) && !isnan(humedad)) {
    enviarDatos(temperatura, humedad); //si encuentra datos de los 2 los manda a enviarDAtos
  } else {
    Serial.println("Error leyendo el sensor DHT11"); // muestra error si no lo esta funcionando
  }

  delay(10000); // Esperar 10 segundos antes del siguiente envío
}

void enviarDatos(float temp, float hum) { //funcionalidad para el backend
  if (WiFi.status() == WL_CONNECTED) { //verifica si sigue conectado al wifi
    HTTPClient http; //crea el cliente


  String url = "http://" + String(serverIPv4) + ":" + String(serverPort) + "/api/datos"; //se construye la url del endpoint

   http.begin(url);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"temperatura\": " + String(temp) + ", \"humedad\": " + String(hum) + "}"; //arma el json
    Serial.println("Enviando datos a: " + url);
    Serial.println("Payload: " + payload);

    int response = http.POST(payload); // manda el post 

    if (response > 0) {
      Serial.print("Respuesta del servidor: "); // si el servidor recibe correctamente manda un 200
      Serial.println(response);
      String cuerpo = http.getString();
      Serial.println("Contenido: " + cuerpo);
    } else {
      Serial.print("Error al enviar POST: "); // si da error manda un -1
      Serial.println(response);
    }

    http.end();
  } else {
    Serial.println("No hay conexión Wi-Fi");
  }
}
