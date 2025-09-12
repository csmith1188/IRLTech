#include <ESP8266WiFi.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>

// Socket.IO server details
const char* host = "formbeta.yorktechapps.com";
const int port = 421;
const char* apiKey = "90fe3b567415813e82b545fd6891c581ad6b795f66380fa338184b556725e3f4";

SocketIOclient socketIO;
unsigned long lastReconnectAttempt = 0;
unsigned long lastHeartbeat = 0;
const unsigned long reconnectInterval = 5000; // Try to reconnect every 5 seconds
const unsigned long heartbeatInterval = 25000; // Send heartbeat every 25 seconds

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println();

    // Connect to WiFi using saved credentials
    WiFi.begin();
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup Socket.IO
    socketIO.begin(host, port, "/socket.io/?EIO=3");
    
    // Event handlers
    socketIO.onEvent(socketIOEvent);
}

void loop() {
    socketIO.loop();

    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        WiFi.reconnect();
        delay(5000);
        return;
    }

    // Handle Socket.IO reconnection
    if (!socketIO.isConnected()) {
        unsigned long currentMillis = millis();
        if (currentMillis - lastReconnectAttempt >= reconnectInterval) {
            lastReconnectAttempt = currentMillis;
            Serial.println("Socket.IO connection lost. Reconnecting...");
            socketIO.begin(host, port, "/socket.io/?EIO=4");
        }
    } else {
        // Send periodic heartbeat to keep connection alive
        unsigned long currentMillis = millis();
        if (currentMillis - lastHeartbeat >= heartbeatInterval) {
            lastHeartbeat = currentMillis;
            String heartbeatMsg = "42[\"heartbeat\"]";
            socketIO.send(sIOtype_EVENT, (uint8_t*)heartbeatMsg.c_str(), heartbeatMsg.length());
        }
    }
}

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
    String msg;
    
    switch(type) {
        case sIOtype_DISCONNECT:
            Serial.println("Disconnected!");
            break;
            
        case sIOtype_CONNECT:
            Serial.println("Connected!");
            delay(200); 
            // Send API key in headers
            msg = "42[\"authenticate\",{\"api\":\"" + String(apiKey) + "\"}]";
            socketIO.send(sIOtype_EVENT, (uint8_t*)msg.c_str(), msg.length());
            delay(200);
            // Request active class
            socketIO.send(sIOtype_EVENT, (uint8_t*)"42[\"getActiveClass\"]", 21);
            break;
            
        case sIOtype_EVENT:
            msg = String((char*)payload);
            if(msg.indexOf("setClass") != -1) {
                // Parse the class ID from the message
                int startIndex = msg.indexOf("[") + 1;
                int endIndex = msg.indexOf("]");
                String classId = msg.substring(startIndex, endIndex);
                Serial.print("The user is currently in the class with id ");
                Serial.println(classId);
            } else if(msg.indexOf("heartbeat") != -1) {
                Serial.println("Heartbeat received");
            }
            break;

        case sIOtype_ERROR:
            Serial.print("Socket.IO Error: ");
            Serial.println((char*)payload);
            break;

        case sIOtype_BINARY_EVENT:
        case sIOtype_BINARY_ACK:
            Serial.println("Received binary data");
            break;
    }
}
