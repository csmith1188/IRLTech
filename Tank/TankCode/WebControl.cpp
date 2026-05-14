#include "Tank.h"

const char* ssid = "robonet";
const char* password = "formDog220!";

WebServer server(80);

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Tank Control</title>

<style>



body{
font-family:Arial;
text-align:center;
background:#181818;
color:white;
}

button{
width:120px;
height:60px;
font-size:20px;
margin:10px;
border:none;
border-radius:10px;
background:#4caf50;
color:white;
}

.slider{
width:300px;
accent-color:#4caf50;
}

#canvas {
      width: 95vw; height: 100vh; position: relative;
      background: #181818; border: 2px solid black; overflow: hidden;
    }

    .block {
  width: 140px;
  padding: 14px 10px;
  background: #4caf50;
  color: white;
  text-align: center;
  border-radius: 8px;
  position: absolute;
  cursor: grab;
  user-select: none;

  position: absolute;
}

/* TOP DIVOT (cutout) */
.block::before {
  content: "";
  position: absolute;
  top: -8px;
  left: 50%;
  transform: translateX(-50%);
  width: 40px;
  height: 16px;
  background: #181818;
  border-radius: 0 0 12px 12px;
  z-index: 0;
}

/* BOTTOM CONNECTOR (half circle tab) */
.block::after {
  content: "";
  position: absolute;
  bottom: -8px;
  left: 50%;
  transform: translateX(-50%);
  width: 40px;
  height: 16px;
  background: #4caf50;
  border-radius: 12px 12px 0 0;
  z-index: 1;
}

    .highlight { outline: 3px dashed #333; }

    #output {
      position: absolute; bottom: 10px; left: 10px;
      background: black; color: #4caf50; padding: 10px;
      font-size: 12px; border-radius: 6px;
    }


</style>
</head>

<body>

<h1>Tank Control</h1>

<button onclick=spawnBlock('Move')>Forward</button>
<br>

<button onclick=spawnBlock('Left')>Left</button>
<button onclick="send('s')">Stop</button>
<button onclick=spawnBlock('Right')>Right</button>

<br>

<button onclick=spawnBlock('Backward')>Backward</button>

<br><br>

<h3>Speed</h3>

<input type="range" min="175" max="255" value="175"
class="slider" onchange="speed(this.value)">

<br><br>

<h2>IR Control</h2>

<button onclick="sendIR('on')">Shoot</button>

<br><br>

<h2>Turret</h2>

<button onclick="turret('left')">Left</button>
<button onclick="turret('center')">Center</button>
<button onclick="turret('right')">Right</button>
<br><br>

<button onclick=startOrder()>Run Code</button>
<button onclick=

<div id="canvas">
  <div class="block" data-name="Move" style="left: 50px; top: 50px;">Move</div>
  <div class="block" data-name="Left" style="left: 220px; top: 50px;">Left</div>
  <div class="block" data-name="Right" style="left: 390px; top: 50px;">Right</div>
  <div class="block" data-name="Backward" style="left: 560px; top: 50px;">Backward</div>


  <div id="output">Order: []</div>
</div>

<script>
const canvas = document.getElementById("canvas");
const output = document.getElementById("output");

let dragged = null;
let offsetX = 0;
let offsetY = 0;

/* =========================
   DRAG SYSTEM (GLOBAL)
========================= */
document.addEventListener('mousedown', (e) => {
  if (!e.target.classList.contains('block')) return;

  dragged = e.target;
  offsetX = e.clientX - dragged.offsetLeft;
  offsetY = e.clientY - dragged.offsetTop;
  dragged.style.cursor = 'grabbing';
});

document.addEventListener('mousemove', (e) => {
  if (!dragged) return;

  const newX = e.clientX - offsetX;
  const newY = e.clientY - offsetY;

  const dx = newX - dragged.offsetLeft;
  const dy = newY - dragged.offsetTop;

  moveChain(dragged, dx, dy);
});

document.addEventListener('mouseup', () => {
  if (!dragged) return;

  const blocks = document.querySelectorAll('.block');
  let bestTarget = null;
  let bestDistance = Infinity;

  blocks.forEach(b => {
    if (b === dragged) return;

    const dx = Math.abs(dragged.offsetLeft - b.offsetLeft);
    const dy = Math.abs(dragged.offsetTop - (b.offsetTop + b.offsetHeight));
    const dist = dx + dy;

    if (dx < 80 && dy < 40 && dist < bestDistance) {
      bestDistance = dist;
      bestTarget = b;
    }
  });

  // detach from previous chain
  if (dragged.prev) {
    dragged.prev.next = null;
    dragged.prev = null;
  }

  // attach to new block
  if (bestTarget) {
    bestTarget.next = dragged;
    dragged.prev = bestTarget;

    snapChain(bestTarget);
  }

  dragged = null;
  updateOrder();
});

/* =========================
   BLOCK CREATION
========================= */
function createBlock(name) {
  let block = document.createElement("div");
  block.className = "block";
  block.dataset.name = name;

  block.textContent = name;

  block.style.left = Math.random() * 400 + 'px';
  block.style.top = Math.random() * 200 + 'px';

  // chain links
  block.next = null;
  block.prev = null;

  addDeleteEvent(block);
  return block;
}

function spawnBlock(name) {
  const block = createBlock(name);
  canvas.appendChild(block);
  updateOrder();
}

/* =========================
   DELETE (RIGHT CLICK)
========================= */
function addDeleteEvent(block) {
  block.addEventListener('contextmenu', (e) => {
    e.preventDefault();

    // reconnect chain
    if (block.prev) block.prev.next = block.next;
    if (block.next) block.next.prev = block.prev;

    block.remove();
    updateOrder();
  });
}

/* =========================
   ORDER SYSTEM
========================= */
function updateOrder() {
  const blocks = document.querySelectorAll('.block');

  const ordered = Array.from(blocks)
    .sort((a, b) => a.offsetTop - b.offsetTop)
    .map(b => b.dataset.name);

  output.textContent = 'Order: ' + JSON.stringify(ordered);
  window.blockOrder = ordered;
}

/* =========================
   CHAIN SYSTEM
========================= */
function snapChain(block) {
  let current = block;

  while (current.next) {
    const next = current.next;

    next.style.left = current.offsetLeft + 'px';
    next.style.top = (current.offsetTop + current.offsetHeight) + 'px';

    current = next;
  }
}

function moveChain(block, dx, dy) {
  let current = block;

  while (current) {
    current.style.left = (current.offsetLeft + dx) + 'px';
    current.style.top = (current.offsetTop + dy) + 'px';
    current = current.next;
  }
}

/* =========================
   EXECUTION SYSTEM
========================= */
function delay(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function startOrder() {
  for (const block of window.blockOrder) {

    if (block === 'Move') {
      send("f");
    } else if (block === 'Right') {
      send("r");
    } else if (block === 'Left') {
      send("l");
    } else if (block === 'Backward') {
      send("b");
    }

    await delay(2000);
    send("s");
    await delay(500);
  }
}

/* =========================
   EXISTING FUNCTIONS
========================= */
function send(cmd){
  fetch("/move?dir=" + cmd);
}

function speed(val){
  fetch("/speed?val=" + val);
}

function sendIR(state){
  fetch("/ir?state=" + state);
}

function turret(dir){
  fetch("/turret?dir=" + dir);
}

/* =========================
   INIT
========================= */
updateOrder();
</script>

</body>
</html>
)rawliteral";

void initWiFi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void initWebServer() {

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  server.on("/move", []() {
    String dir = server.arg("dir");

    if (dir == "f") forward();
    else if (dir == "b") backward();
    else if (dir == "l") left();
    else if (dir == "r") right();
    else if (dir == "s") stopMotors();

    server.send(200, "text/plain", "OK");
    Serial.println("Move command: " + dir);
  });

  server.on("/speed", []() {
    motorSpeed = server.arg("val").toInt();
    server.send(200, "text/plain", "Speed Set");
  });

  server.on("/ir", []() {
    String state = server.arg("state");

    if (state == "on") sendIROn();

    server.send(200, "text/plain", "IR Sent");
  });

  server.on("/turret", []() {
    String dir = server.arg("dir");

    if (dir == "left") turretLeft();
    else if (dir == "right") turretRight();
    else if (dir == "center") turretCenter();

    server.send(200, "text/plain", "Turret OK");

    Serial.println("Turret command received: " + dir);
  });

  server.begin();
}

void initOTA() {

  ArduinoOTA.setHostname("ESP32-Tank");
  ArduinoOTA.begin();
}

void handleWebClient() {

  server.handleClient();
}

void handleOTA() {

  ArduinoOTA.handle();
}