#include "Tank.h"

const char* ssid = "robonet";
const char* password = "formDog220!";

WebServer server(80);

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 Tank</title>

<style>

body{
font-family:Arial;
text-align:center;
background:#111;
color:white;
}

button{
width:120px;
height:60px;
font-size:20px;
margin:10px;
border:none;
border-radius:10px;
background:#00bcd4;
color:white;
}

.slider{
width:300px;
}

</style>
</head>

<body>

<h1>ESP32 Tank Control</h1>

<button onclick="send('f')">Forward</button>
<br>

<button onclick="send('l')">Left</button>
<button onclick="send('s')">Stop</button>
<button onclick="send('r')">Right</button>

<br>

<button onclick="send('b')">Backward</button>

<br><br>

<h3>Speed</h3>

<input type="range" min="175" max="255" value="175"
class="slider" onchange="speed(this.value)">

<br><br>

<h2>IR Control</h2>

<button onclick="sendIR('on')">IR ON</button>
<button onclick="sendIR('off')">IR OFF</button>

<script>

function send(cmd){
fetch("/move?dir=" + cmd);
}

function speed(val){
fetch("/speed?val=" + val);
}

function sendIR(state){
fetch("/ir?state=" + state);
}

</script>

</body>
</html>
)rawliteral";

void initWiFi(){

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting");

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

}

void initWebServer(){

  server.on("/", [](){
    server.send(200,"text/html",webpage);
  });

  server.on("/move", [](){

    String dir = server.arg("dir");

    if(dir == "f") forward();
    else if(dir == "b") backward();
    else if(dir == "l") left();
    else if(dir == "r") right();
    else if(dir == "s") stopMotors();

    server.send(200,"text/plain","OK");

  });

  server.on("/speed", [](){

    motorSpeed = server.arg("val").toInt();
    server.send(200,"text/plain","Speed Set");

  });

  server.on("/ir", [](){

    String state = server.arg("state");

    if(state == "on") sendIROn();
    else if(state == "off") sendIROff();

    server.send(200,"text/plain","IR Sent");
  });

  server.begin();

}

void initOTA(){

  ArduinoOTA.setHostname("ESP32-Tank");
  ArduinoOTA.begin();

}

void handleWebClient(){

  server.handleClient();

}

void handleOTA(){

  ArduinoOTA.handle();

}