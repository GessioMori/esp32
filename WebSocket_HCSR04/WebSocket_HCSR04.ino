

// Import required libraries
#include <WiFi.h>
#include <Ultrasonic.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "NET_2GCA23D3";
const char* password = "4BCA23D3";

const int trigPin = 26;
const int echoPin = 25;
int distance;

// Create Ultrasonic object
Ultrasonic ultrasonic(trigPin, echoPin);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  .topnav {
    overflow: hidden;
    background-color: #453b37;
  }
  body {
    margin: 0;
    background-color: #212222; 
    color: #f3f5f6;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
    color: #f3f5f6;
  }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server - HC-SR04</h1>
  </div>
  <div class="content">
    <p>Distância: <span id="distance">%DISTANCE%</span> cm</p>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  var distance;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    distance = event.data;
    document.getElementById('distance').innerHTML = distance;
  }
  function onLoad(event) {
    initWebSocket();
  }
</script>
</body>
</html>
)rawliteral";

void initWebSocket() {
  server.addHandler(&ws);
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  distance = ultrasonic.read();
  ws.textAll(String(distance));
  delay(100);
}
