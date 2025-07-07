# BEHEMOTH - Automatic Vegetable Transplanter

![BEHEMOTH Logo](img/behemoth-logo.png)

> An intelligent tractor attachment for automated vegetable transplanting, developed by the SAE team at CUSAT (Cochin University of Science and Technology).

## 📖 Overview

BEHEMOTH is an advanced automatic vegetable transplanter attachment designed for tractors, capable of precisely transplanting seedlings for various crops including tomatoes, brinjal (eggplant), and chili peppers. The system features intelligent spacing control, real-time monitoring, and a user-friendly web interface for operation and management.

![BEHEMOTH Tractor Attachment](img/tractor-attachment.jpg)

## 🌟 Key Features

### 🚜 Agricultural Capabilities
- **Multi-crop support**: Tomato (300mm spacing), Brinjal (450mm spacing), Chilly (600mm spacing)
- **Precision transplanting**: Servo-controlled seedling placement
- **Adaptive spacing**: Stepper motor-controlled bed-to-bed distance adjustment
- **Real-time plant counting**: Automated transplant tracking

### 🎛️ Control Systems
- **Dual operation modes**: Manual joystick control and automated sensor-based operation
- **PIR sensor integration**: Motion detection for precise timing
- **Emergency stop system**: Immediate safety shutdown capability
- **Water level monitoring**: Automated irrigation system tracking

### 📱 Smart Interface
- **WiFi-enabled**: ESP32-powered wireless connectivity
- **Mobile-responsive web app**: Real-time monitoring and control
- **Hire tracking system**: Time and cost calculation for commercial use
- **Live sensor data**: Plant count, water levels, and system status

![Web Interface Screenshot](img/web-interface.png)

## 🔧 Technical Specifications

### Hardware Components
- **Microcontroller**: ESP32 WiFi Module
- **Motors**: 
  - 2x Stepper Motors (TB6600 drivers, 4800 steps/revolution)
  - 2x Servo Motors for seedling placement
- **Sensors**:
  - 2x PIR Motion Sensors
  - Plant count proximity sensor
  - Water level sensor
  - Tray end detection sensor
- **Control**: Analog joystick with push button
- **Safety**: Emergency stop relay system
- **Audio**: Buzzer for notifications

### Software Features
- **Real-time web interface** with responsive design
- **Persistent data storage** using ESP32 preferences
- **WiFi Access Point mode** (SSID: BEHEMOTH_AP)
- **Automated hire tracking** with SMS integration
- **Live sensor monitoring** with 3-second update intervals

## 🚀 Getting Started

### Prerequisites
- Arduino IDE with ESP32 board support
- Required libraries:
  - `WiFi.h`
  - `ESPAsyncWebServer.h`
  - `Preferences.h`
  - `ESP32Servo.h`

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Deratheone/Behemoth.git
   cd Behemoth
   ```

2. **Upload the firmware**:
   - Open `code/behemoth.ino` in Arduino IDE
   - Select ESP32 board and appropriate COM port
   - Upload the code to your ESP32

3. **Connect to the system**:
   - Power on the BEHEMOTH system
   - Connect your device to WiFi network: `BEHEMOTH_AP`
   - Password: `behemoth123`
   - Open browser and navigate to: `192.168.4.1`

### Hardware Setup

![Hardware Connection Diagram](img/hardware-diagram.png)

#### Pin Connections:
- **Stepper Motor 1**: PUL=18, DIR=19
- **Stepper Motor 2**: PUL=21, DIR=22
- **Servo Motors**: GPIO 4, GPIO 5
- **PIR Sensors**: GPIO 2, GPIO 15
- **Joystick**: X=34, Y=35, Button=33
- **Proximity Sensors**: Plant Count=12, Water Level=13, Tray End=14
- **Emergency Stop**: GPIO 4 (relay control)
- **Buzzer**: GPIO 27

## 💼 Commercial Features

### Hire System
- **Automated billing**: ₹1.0 per minute usage tracking
- **Session management**: Start/stop hire with persistent storage
- **SMS integration**: Automatic billing summary via SMS
- **Real-time monitoring**: Live duration and cost calculation

### Monitoring Dashboard
- **Plant transplant counter**: Real-time count display
- **Water level indicator**: Percentage-based with visual progress bar
- **System alerts**: Tray end notifications and low water warnings
- **Emergency controls**: One-touch emergency stop functionality

## 🎮 Operation Modes

### 1. Manual Mode
- **Joystick control**: Direct stepper motor manipulation
- **Real-time feedback**: Immediate response to operator input
- **Safety override**: Emergency stop always active

### 2. Automatic Mode
- **Sensor-driven operation**: PIR sensors trigger transplanting sequence
- **Predefined patterns**: Crop-specific spacing and movement
- **Autonomous operation**: Minimal operator intervention required

## 📊 Monitoring & Analytics

![System Monitoring](img/monitoring-dashboard.png)

- **Real-time metrics**: Plant count, water levels, system status
- **Performance tracking**: Transplanting rate and efficiency
- **Alert system**: Visual and audio notifications for system events
- **Data persistence**: All critical data stored in non-volatile memory

## 👥 Development Team

![SAE Team CUSAT](img/team-photo.jpg)

Developed by the **Society of Automotive Engineers (SAE) Team** at **Cochin University of Science and Technology (CUSAT)** as part of innovative agricultural automation research.

### Project Contributors
- **Development Intern**: [Your Name] - Lead Developer & System Integration
- **SAE Team CUSAT** - Project guidance and support
- **Faculty Advisors** - Technical oversight and validation

## 🏆 Project Achievements

- ✅ Successfully implemented automated transplanting for multiple crop types
- ✅ Developed intuitive web-based control interface
- ✅ Integrated commercial hire tracking system
- ✅ Achieved precise spacing control with stepper motor accuracy
- ✅ Implemented comprehensive safety and monitoring systems

## 📈 Future Enhancements

- [ ] **GPS Integration**: Field mapping and area coverage tracking
- [ ] **Machine Learning**: Crop recognition and adaptive spacing
- [ ] **IoT Connectivity**: Cloud-based monitoring and remote control
- [ ] **Mobile App**: Dedicated smartphone application
- [ ] **Database Integration**: Historical data analysis and reporting

## 🛠️ Technical Documentation

### API Endpoints
- `GET /` - Main web interface
- `GET /wifi-status` - WiFi connection status
- `GET /hire-status` - Current hire session status
- `POST /start-hire` - Begin hire session
- `POST /end-hire` - End hire session and calculate billing
- `GET /sensor-data` - Real-time sensor readings
- `POST /emergency-stop` - Emergency system shutdown

### Configuration
```cpp
// WiFi Credentials
const char* ssid = "BEHEMOTH_AP";
const char* password = "behemoth123";

// Crop Spacing Settings
// Tomato: 300mm bed-to-bed distance
// Brinjal: 450mm bed-to-bed distance  
// Chilly: 600mm bed-to-bed distance

// Billing Rate
const float HOURLY_RATE = 1.0; // Rupees per minute
```

## 📄 License

This project is developed for educational and research purposes as part of the SAE team activities at CUSAT.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bug reports and feature requests.

## 📞 Contact

For questions, support, or collaboration opportunities, please contact:
- **SAE Team CUSAT**: [University Contact Information]
- **Project Repository**: [GitHub Repository Link]

---

**Built with ❤️ by SAE Team CUSAT for the future of precision agriculture**

![CUSAT Logo](img/cusat-logo.png) ![SAE Logo](img/sae-logo.png)