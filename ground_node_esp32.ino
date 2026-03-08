#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <map>
#define LED 2


AsyncWebServer server(80);
AsyncEventSource events("/events");

struct SensorData {
  float temp;
  float hum;
  int mq2;
  int mq135;
  int vibration;
} incomingData;

std::map<String, unsigned long> onlineNodes;

unsigned long ledOffTime = 0;
const int blinkDuration = 50;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>SynapseNET | Command Center</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@400;600;800&display=swap" rel="stylesheet">
    <style>
        :root { --bg: #f8fafc; --card: #ffffff; --text: #1e293b; --primary: #2563eb; --border: #e2e8f0; }
        [data-theme='dark'] { --bg: #0f172a; --card: #1e293b; --text: #f8fafc; --primary: #38bdf8; --border: #334155; }
        
        body { font-family: 'Plus Jakarta Sans', sans-serif; background: var(--bg); color: var(--text); transition: 0.3s; margin: 0; padding: 20px; }
        .dashboard { max-width: 1200px; margin: 0 auto; }
        
        header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 30px; border-bottom: 2px solid var(--border); padding-bottom: 20px; }
        .node-badge { background: var(--primary); color: white; padding: 6px 16px; border-radius: 30px; font-size: 0.85rem; font-weight: 600; transition: background 0.5s ease; }
        
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(450px, 1fr)); gap: 20px; }
        .card { background: var(--card); padding: 20px; border-radius: 20px; border: 1px solid var(--border); box-shadow: 0 4px 6px -1px rgba(0,0,0,0.05); }
        
        .label { font-size: 0.75rem; font-weight: 700; color: #64748b; text-transform: uppercase; letter-spacing: 0.5px; margin-bottom: 10px; }
        .number { font-size: 2.2rem; font-weight: 800; color: var(--primary); }
        .unit { font-size: 1rem; opacity: 0.7; margin-left: 5px; }
        
        .btn-theme { cursor: pointer; border: none; background: #64748b22; color: var(--text); padding: 8px 16px; border-radius: 12px; font-weight: 600; font-size: 0.8rem; }
        .chart-container { height: 160px; margin-top: 15px; position: relative; }
        .status-pill { padding: 12px; border-radius: 14px; text-align: center; font-weight: 800; font-size: 0.9rem; margin-top: 15px; transition: 0.5s; }
    </style>
</head>
<body data-theme="light">
    <div class="dashboard">
        <header>
            <div style="display:flex; align-items:center; gap:10px;">
                <div style="width:12px; height:12px; background:#10b981; border-radius:50%; animation: pulse 2s infinite;"></div>
                <h1 style="margin:0; font-size:1.4rem;">SynapseNET Dashboard</h1>
            </div>
            <div style="display:flex; gap:10px; align-items:center;">
                <div id="nodeList" class="node-badge">Searching...</div>
                <button class="btn-theme" onclick="toggleTheme()">Theme</button>
            </div>
        </header>

        <div class="grid">
            <div class="card">
                <div class="label">Temperature</div>
                <div><span id="temp" class="number">--</span><span class="unit">°C</span></div>
                <div class="chart-container"><canvas id="tempChart"></canvas></div>
            </div>
            
            <div class="card">
                <div class="label">Air Humidity</div>
                <div><span id="hum" class="number">--</span><span class="unit">%</span></div>
                <div class="chart-container"><canvas id="humChart"></canvas></div>
            </div>

            <div class="card">
                <div class="label">Smoke & Leak Detection</div>
                <div style="font-size: 0.8rem; color:#64748b;">(Flammable Gas Intensity)</div>
                <div class="chart-container"><canvas id="smokeChart"></canvas></div>
            </div>

            <div class="card">
                <div class="label">General Air Purity</div>
                <div style="font-size: 0.8rem; color:#64748b;">(Pollution & CO2 Levels)</div>
                <div class="chart-container"><canvas id="purityChart"></canvas></div>
            </div>

            <div class="card" style="display:flex; flex-direction:column; justify-content:center;">
                <div class="label">Structural Stability</div>
                <div id="vibBox" class="status-pill" style="background: var(--border);">WAITING FOR DATA</div>
                <p style="font-size:0.75rem; color:#94a3b8; text-align:center; margin-top:15px;">Monitors movement and seismic activity.</p>
            </div>
        </div>
    </div>

    <script>
        function toggleTheme() {
            const body = document.body;
            body.setAttribute('data-theme', body.getAttribute('data-theme') === 'light' ? 'dark' : 'light');
        }

        const chartOptions = {
            responsive: true,
            maintainAspectRatio: false,
            plugins: { legend: { display: false } },
            scales: { x: { display: false }, y: { grid: { display: false }, ticks: { display: false } } }
        };

        function setupLine(id, color) {
            return new Chart(document.getElementById(id), {
                type: 'line',
                data: { labels: Array(15).fill(''), datasets: [{ data: [], borderColor: color, tension: 0.4, pointRadius: 0, fill: true, backgroundColor: color+'11' }] },
                options: chartOptions
            });
        }

        function setupBar(id, color) {
            return new Chart(document.getElementById(id), {
                type: 'bar',
                data: { labels: ['Level'], datasets: [{ data: [0], backgroundColor: color, borderRadius: 10, barThickness: 85 }] },
                options: { ...chartOptions, scales: { y: { min:0, max:1024, display: false } } }
            });
        }

        const tChart = setupLine('tempChart', '#3b82f6');
        const hChart = setupLine('humChart', '#10b981');
        const sChart = setupBar('smokeChart', '#ef4444');
        const pChart = setupBar('purityChart', '#8b5cf6');

        if (!!window.EventSource) {
            var source = new EventSource('/events');
            source.addEventListener('new_readings', function(e) {
                var obj = JSON.parse(e.data);
                lastDataTime = Date.now();

                const badge = document.getElementById("nodeList");
                badge.style.background = "var(--primary)";

                document.getElementById("temp").innerText = obj.temp;
                document.getElementById("hum").innerText = obj.hum;
                
                // Update Lines
                [tChart, hChart].forEach((c, i) => {
                    c.data.datasets[0].data.push(i==0 ? obj.temp : obj.hum);
                    if(c.data.datasets[0].data.length > 15) c.data.datasets[0].data.shift();
                    c.update('none');
                });

                // Update Bars
                sChart.data.datasets[0].data = [obj.mq2];
                pChart.data.datasets[0].data = [obj.mq135];
                sChart.update();
                pChart.update();

                // Vibration Logic
                const v = document.getElementById("vibBox");
                v.innerText = obj.vibration == 1 ? "VIBRATION DETECTED" : "SYSTEM STABLE";
                v.style.background = obj.vibration == 1 ? "#ef4444" : "#10b981";
                v.style.color = "white";

                document.getElementById("nodeList").innerText = obj.nodes + " Node(s) Online";
            });
        }
        setInterval(() => {
            const badge = document.getElementById("nodeList");
            if (Date.now() - lastDataTime > 15000) {
                badge.innerText = "Connection Lost";
                badge.style.background = "#ef4444";
            }
        }, 5000);

        var lastDataTime = Date.now();
    </script>
    <style> @keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.4; } 100% { opacity: 1; } } </style>
</body>
</html>
)rawliteral";

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingBytes, int len) {
  digitalWrite(LED, HIGH);
  ledOffTime = millis() + blinkDuration;
  memcpy(&incomingData, incomingBytes, sizeof(incomingData));
  
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  onlineNodes[String(macStr)] = millis();

  int activeCount = 0;
  for (auto const& [key, val] : onlineNodes) {
    if (millis() - val < 10000) activeCount++;
  }

  String json = "{";
  json += "\"temp\":" + String(incomingData.temp) + ",";
  json += "\"hum\":" + String(incomingData.hum) + ",";
  json += "\"mq2\":" + String(incomingData.mq2) + ",";
  json += "\"mq135\":" + String(incomingData.mq135) + ",";
  json += "\"vibration\":" + String(incomingData.vibration) + ",";
  json += "\"nodes\":" + String(activeCount);
  json += "}";
  
  events.send(json.c_str(), "new_readings", millis());
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  WiFi.softAP("SynapseNET", "12345678");
  if (esp_now_init() != ESP_OK) return;
  esp_now_register_recv_cb(OnDataRecv);
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.addHandler(&events);
  server.begin();
}

void loop() 
{
    if (millis() >= ledOffTime && digitalRead(LED) == HIGH) {
    digitalWrite(LED, LOW);
  }
}
