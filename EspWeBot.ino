#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "iqhtyar";
const char* password = "12345678";

AsyncWebServer server(80);

// GPIO pin for control
const int gpioPin = 2;

// Function to handle GPIO control requests
void handleGpioControl(AsyncWebServerRequest *request) {
  if (request->hasArg("gpio")) {
    String gpioValue = request->arg("gpio");
    if (gpioValue == "on") {
      digitalWrite(gpioPin, HIGH);
      request->send(200, "text/plain", "GPIO turned ON");
    } else if (gpioValue == "off") {
      digitalWrite(gpioPin, LOW);
      request->send(200, "text/plain", "GPIO turned OFF");
    } else {
      request->send(400, "text/plain", "Invalid request");
    }
  } else {
    request->send(400, "text/plain", "Missing 'gpio' parameter");
  }
}

void setup() {
  pinMode(gpioPin, OUTPUT);
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Serve the chatbot HTML file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String htmlContent = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>AiChatBot</title><style>#chat-container{max-width:300px;margin:auto;border:1px solid #ccc;padding:10px;border-radius:10px;}</style></head><body><p>Mau tanya apa d^_^b:</p><input id='input' type='text' placeholder='Type here'><button onclick='askQuestion()'>Ask</button><p id='response'></p><script>";
    htmlContent += "var storedAnswers = JSON.parse(localStorage.getItem(\"chatbotAnswers\")) || {};";
    htmlContent += "function saveToLocalStorage() {localStorage.setItem(\"chatbotAnswers\", JSON.stringify(storedAnswers));}";
    htmlContent += "function askQuestion() {var question = document.getElementById(\"input\").value;";
    htmlContent += "if (question.trim().toLowerCase() === \"turnon\") {turnOn();document.getElementById(\"response\").innerHTML = \"GPIO turned ON\";return;} else if (question.trim().toLowerCase() === \"turnoff\") {turnOff();document.getElementById(\"response\").innerHTML = \"GPIO turned OFF\";return;}";
    htmlContent += "if (storedAnswers.hasOwnProperty(question)) {document.getElementById(\"response\").innerHTML = storedAnswers[question];} else {var userAnswer = prompt(\"Maaf, aku enggak mengerti pertanyaan kamu. Aku harus jawab apa ya? : \");storedAnswers[question] = \"d^_^b  \" + userAnswer;document.getElementById(\"response\").innerHTML = storedAnswers[question];saveToLocalStorage();}}";
    htmlContent += "function turnOn() {var xhr = new XMLHttpRequest();xhr.open(\"GET\", \"/gpio?gpio=on\");xhr.send();}";
    htmlContent += "function turnOff() {var xhr = new XMLHttpRequest();xhr.open(\"GET\", \"/gpio?gpio=off\");xhr.send();}</script>";
    htmlContent += "<button onclick='turnOn()'>Turn GPIO ON</button><button onclick='turnOff()'>Turn GPIO OFF</button></body></html>";

    request->send(200, "text/html", htmlContent);
  });

  // Handle GPIO control requests
  server.on("/gpio", HTTP_GET, handleGpioControl);

  server.begin();
}

void loop() {
  // Nothing to do here, as the server handles requests asynchronously
}
