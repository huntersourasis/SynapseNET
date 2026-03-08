# SynapseNET | Decentralized Subterranean Atmospheric Monitoring

**SynapseNET** is a high-performance, decentralized communication network designed for real-time environmental monitoring. By leveraging the **ESP-NOW protocol**, the system creates a **wireless mesh network** capable of transmitting critical sensor data across long distances **without requiring a Wi-Fi router or internet connection**.



---

## 🚀 Key Features

* **Zero-Infrastructure Communication:** Uses **ESP-NOW (2.4GHz)** for ultra-fast peer-to-peer communication between nodes.
* **Decentralized Mesh Architecture:** Implements **Bridge Nodes** that receive and rebroadcast packets to extend range.
* **Professional HQ Dashboard:** A mobile-responsive web dashboard hosted directly on the **ESP32 Ground Station**.
* **Adaptive User Interface:** Features a modern **Dark Mode** for high-visibility monitoring.
* **Real-Time Analytics:** Live-updating charts powered by **Chart.js** visualize telemetry data instantly.
* **Fail-Safe Monitoring:** Built-in **node health monitoring** detects offline transmitters and triggers **Connection Lost** alerts.

---

## 🏗️ System Architecture

The network operates across **three functional layers**:

### 1. Transmitter Node (ESP8266 / ESP32)
Collects environmental data and broadcasts it using a **compact data struct**.
* **Sensors:** MQ-2 (Gas), MQ-135 (Air Quality), DHT (Temp/Humidity), and Vibration sensors.

### 2. Bridge Node (ESP8266)
Acts as a **wireless repeater** to extend the mesh coverage.
* **Functions:** Receives raw telemetry packets and rebroadcasts them across the network.

### 3. Ground Station (ESP32 HQ)
The **central intelligence layer** of the system.
* **Responsibilities:** Converts raw packets to **JSON**, hosts the **Asynchronous Web Server**, and manages the live UI.



---

## 📊 Monitored Metrics

| Metric | Sensor Type | Purpose |
| :--- | :--- | :--- |
| **Atmospheric Temp** | DHT / Digital | Monitoring ambient heat levels |
| **Relative Humidity** | DHT / Digital | Tracking moisture for structural safety |
| **Smoke / Gas Leak** | MQ-2 | Detecting flammable gases or smoke |
| **Air Quality / CO₂** | MQ-135 | Measuring pollution levels |
| **Structural Stability** | Vibration | Detecting seismic activity or stress |

---

## 🛠️ Tech Stack

* **Firmware:** C++ (Arduino Framework)
* **Communication:** ESP-NOW Protocol (Low-Latency)
* **Frontend:** HTML5, CSS3 (Glassmorphism UI), JavaScript (ES6)
* **Data Viz:** Chart.js
* **Server:** ESPAsyncWebServer (Non-Blocking)

---

## 📡 Network Workflow

1.  **Transmitter** reads sensor values and packages them into a **struct**.
2.  Packet is broadcast via **ESP-NOW**.
3.  **Bridge Nodes** relay the packet to extend the signal.
4.  **Ground Station** receives the data and parses it into **JSON**.
5.  The **Dashboard** updates the charts and status indicators in **real time**.

---

## 🛠️ Installation & Setup

### 1. Hardware Wiring
Connect sensors to the **Transmitter Node** according to the GPIO definitions specified inside the firmware code.

### 2. Flash Firmware
Upload the appropriate firmware to each device:
* `sensor_node_esp32.ino`
* `bridge_node_esp8266.ino`
* `ground_node_esp32.ino`

### 3. Access the Dashboard
1.  Power on the **ESP32 Ground Station**.
2.  Connect to Wi-Fi: **SSID: SynapseNET** | **Password: 12345678**.
3.  Navigate to `http://192.168.4.1` in your browser.

---

## 👨‍💻 Developer

**Sourasis Maity** Backend Developer & Student  
Barasat P.C.S. Govt. High School
