#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>


// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Web server running on port 80
WebServer server(80);

// Pin Declarations (ESP32 GPIOs mapping to D5, D6, D7)
const int led1Pin = 5; // LED 1 (GPIO 5)
const int led2Pin = 18; // LED 2 (GPIO 18 / D6)
const int led3Pin = 19; // LED 3 (GPIO 19 / D7)

// State Variables to track real-time LED status
bool led1State = false;
bool led2State = false;
bool led3State = false;

void handleRoot() {
  String html = "<!DOCTYPE html><html>";

  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>ESP32 Multi-LED Control</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; margin-top: 30px; }";
  html += ".card { background: white; width: 300px; margin: 15px auto; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }";
  html += "button { padding: 10px 20px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer; color: white; margin: 5px; }";
  html += ".btn-on { background-color: #4CAF50; }";
  html += ".btn-off { background-color: #f44336; }";
  html += ".status { font-weight: bold; padding: 3px 8px; border-radius: 4px; }";
  html += ".status-on { background-color: #d4edda; color: #155724; }";
  html += ".status-off { background-color: #f8d7da; color: #721c24; }";
  html += "</style></style>";
  // AJAX script to toggle LEDs and poll status without page reloads
  html += "<script>";
  html += "function toggleLED(route) {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      updateStatus();";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', route, true);";
  html += "  xhttp.send();";
  html += "}";
  
  html += "function updateStatus() {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.onreadystatechange = function() {";
  html += "    if (this.readyState == 4 && this.status == 200) {";
  html += "      var data = JSON.parse(this.responseText);";
  html += "      document.getElementById('st1').innerHTML = data.led1 ? 'ON' : 'OFF';";
  html += "      document.getElementById('st1').className = 'status ' + (data.led1 ? 'status-on' : 'status-off');";
  html += "      document.getElementById('st2').innerHTML = data.led2 ? 'ON' : 'OFF';";
  html += "      document.getElementById('st2').className = 'status ' + (data.led2 ? 'status-on' : 'status-off');";
  html += "      document.getElementById('st3').innerHTML = data.led3 ? 'ON' : 'OFF';";
  html += "      document.getElementById('st3').className = 'status ' + (data.led3 ? 'status-on' : 'status-off');";
  html += "    }";
  html += "  };";
  html += "  xhttp.open('GET', '/status', true);";
  html += "  xhttp.send();";
  html += "}";
  html += "setInterval(updateStatus, 2000);"; // Automatically sync status every 2 seconds
  html += "</script>";
  html += "</head><body onload='updateStatus()'>";

  html += "<h2>ESP32 Multi-LED Control Panel</h2>";

  // LED 1 Controls
  html += "<div><h3>LED 1 (GPIO 5)</h3>";
  html += "<p>Status: <span id='st1' class='status status-off'>OFF</span></p>";
  html += "<button class='btn-on' onclick=\"toggleLED('/led?led_id=1&led_action=on')\">Turn ON</button>";
  html += "<button class='btn-off' onclick=\"toggleLED('/led?led_id=1&led_action=off')\">Turn OFF</button></div>";

  // LED 2 Controls
  html += "<div class='card'><h3>LED 2 (GPIO 18)</h3>";
  html += "<p>Status: <span id='st2' class='status status-off'>OFF</span></p>";
  html += "<button class='btn-on' onclick=\"toggleLED('/led?led_id=2&led_action=on')\">Turn ON</button>";
  html += "<button class='btn-off' onclick=\"toggleLED('/led?led_id=2&led_action=off')\">Turn OFF</button></div>";

  // LED 3 Controls
  html += "<div class='card'><h3>LED 3 (GPIO 19)</h3>";
  html += "<p>Status: <span id='st3' class='status status-off'>OFF</span></p>";
  html += "<button class='btn-on' onclick=\"toggleLED('/led?led_id=3&led_action=on')\">Turn ON</button>";
  html += "<button class='btn-off' onclick=\"toggleLED('/led?led_id=3&led_action=off')\">Turn OFF</button></div>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleStatus() {
  String json = "{";
  json += "\"led1\":" + String(led1State ? "true" : "false") + ",";
  json += "\"led2\":" + String(led2State ? "true" : "false") + ",";
  json += "\"led3\":" + String(led3State ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// Function to handle LED state changes based on the LED ID and desired state
void dynamicHandlingOfLed(int led_id, bool led_state, bool led_volt){
  if (led_id == 1) {
      led1State = led_state; // interaction with webUI
      digitalWrite(led1Pin, led_volt); // interaction with the arduino
  } else if (led_id == 2) {
      led2State = led_state;
      digitalWrite(led2Pin, led_volt);
  } else if (led_id == 3) {
      led3State = led_state;
      digitalWrite(led3Pin, led_volt);
  } else {
      Serial.println("Invalid LED ID");
      server.send(400, "text/plain", "Invalid LED ID");
      return;
  }
  server.send(200, "text/plain", "OK");
}



void setup() {
  Serial.begin(115200);

  // Initialize GPIO pins
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);

  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);

  // WiFi Connection
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());



  server.on("/", handleRoot);  //this initiates the frontend page


  server.on("/led", [](){
    String led_action = server.arg("led_action");
    int led_id = server.arg("led_id").toInt();
    if (led_action == "on" && (led_id >= 1 && led_id <= 3)) {
        dynamicHandlingOfLed(led_id, true, HIGH);
    } else if (led_action == "off" && (led_id >= 1 && led_id <= 3)) {
        dynamicHandlingOfLed(led_id, false, LOW);
    } else {
        server.send(400, "text/plain", "Invalid input for led_id or led_action");
    }

  });
  
  server.on("/status", handleStatus);



  server.begin();
}


// eto yung block of code na paulit ulit na gumagana habang naka on ang board
void loop(){
    server.handleClient();
}