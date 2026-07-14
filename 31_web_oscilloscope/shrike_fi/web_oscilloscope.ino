#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h> // Install "WebSockets" by Markus Sattler from Library Manager

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const int analogInPin = 4; // Use ESP_IO4 on Shrike Fi

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Web Oscilloscope</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background: #0f172a; color: #f8fafc; margin: 0; padding: 20px; }
    h1 { color: #38bdf8; margin-bottom: 5px; }
    p { color: #94a3b8; margin-bottom: 20px; }
    .chart-container { position: relative; height: 60vh; width: 90vw; margin: auto; background: #1e293b; border-radius: 12px; padding: 20px; box-shadow: 0 4px 6px -1px rgb(0 0 0 / 0.1); }
    canvas { display: block; width: 100% !important; height: 100% !important; }
  </style>
</head>
<body>
  <h1>ESP32 Web Oscilloscope</h1>
  <p>Live ADC Readings streamed via WebSockets</p>
  
  <div class="chart-container">
    <canvas id="chart"></canvas>
  </div>

  <script>
    var ctx = document.getElementById('chart').getContext('2d');
    var chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'ADC Value (Pin 4)',
          borderColor: '#38bdf8',
          backgroundColor: 'rgba(56, 189, 248, 0.2)',
          data: [],
          fill: true,
          pointRadius: 0,
          borderWidth: 2,
          tension: 0.4
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        scales: {
          x: { display: false },
          y: { 
            min: 0, 
            max: 4095,
            grid: { color: '#334155' },
            ticks: { color: '#94a3b8' }
          }
        },
        plugins: {
          legend: { labels: { color: '#f8fafc' } }
        }
      }
    });

    var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
    
    ws.onmessage = function(event) {
      var val = parseInt(event.data);
      var time = new Date().toLocaleTimeString();
      
      chart.data.labels.push(time);
      chart.data.datasets[0].data.push(val);
      
      if(chart.data.labels.length > 100) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
      }
      chart.update();
    };
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  webSocket.begin();
}

unsigned long lastSend = 0;
void loop() {
  webSocket.loop();
  server.handleClient();
  
  if (millis() - lastSend > 40) { 
    int adcValue = analogRead(analogInPin);
    webSocket.broadcastTXT(String(adcValue));
    lastSend = millis();
  }
}
