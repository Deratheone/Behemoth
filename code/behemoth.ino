#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>  // For persistent storage
#include <time.h>
#include <ESP32Servo.h>  // Add this with your other libraries

// WiFi credentials for AP mode
const char* ssid = "BEHEMOTH_AP";
const char* password = "behemoth123";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Initialize Preferences for persistent storage
Preferences preferences;
// Motion Control System - Global Variables
Servo servo1;
Servo servo2;

// Stepper 1 pins (TB6600)
const int PUL_PIN = 18;
const int DIR_PIN = 19;

// Stepper 2 pins (TB6600)
const int PUL_PIN_2 = 21;
const int DIR_PIN_2 = 22;

// PIR Sensors
const int pirSensor1 = 2;
const int pirSensor2 = 15;

// Buzzer
const int buzzerPin = 27;

// Joystick pins
const int xAxisPin = 34;
const int yAxisPin = 35;
const int joystickBtnPin = 33;

// State variables
bool manualMode = false;
bool lastBtnState = HIGH;
int activeSensor = 1;
unsigned long lastTriggerTime = 0;
const int debounceDelay = 200;
const int stepsPerRevolution = 4800;
const int pulseHighTime = 150;
const int pulseLowTime = 150;
bool servosRotated = false;
int sensor2TriggerCount = 0;
bool rotateSecondStepper = false;
// Variable to track hire status
bool isHired = false;
unsigned long startHireTime = 0;
unsigned long bootTime = 0;        // Time when the device booted up
const float HOURLY_RATE = 1.0;     // Rupees per minute (changed from 150 per hour)

// Emergency stop pin (change this to your desired GPIO pin)
const int emergencyStopPin = 4;    // GPIO pin for emergency stop relay control
bool emergencyStopActive = false;  // Track emergency stop state
// New proximity sensors
const int plantCountSensor = 12;     // GPIO12 for plant count sensor
const int waterLevelSensor = 13;     // GPIO13 for water level sensor
const int trayEndSensor = 14;        // GPIO14 for tray end detection

// Variables for sensors
int plantCount = 23;                 // Initial plant count
int waterLevel = 75;                 // Initial water level percentage
int trayTriggerCount = 0;            // Counter for tray sensor triggers
bool trayEndAlertActive = false;     // Flag for tray end alert
// Function prototype for updateElapsedTime
void updateElapsedTime();

// HTML content (your provided website with modifications)
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>BEHEMOTH - Agricultural Systems</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    }
    
    :root {
      --bg-gradient: linear-gradient(180deg, #000000 0%, #333333 100%);
      --text-color: #fff;
      --primary-color: #171717;
      --secondary-color: #737373;
      --card-bg: #fff;
      --tomato-color: #333333;
      --brinjal-color: #555555;
      --chilly-color: #777777;
      --status-bg: #f5f5f5;
      --radius: 12px;
      --highlight-color: #ffffff;
      --accent-color: #aaaaaa;
      --start-hire-color: #90EE90; /* Light green */
      --end-hire-color: #FFA07A; /* Light red */
      --disabled-color: #cccccc; /* Grey for disabled buttons */
      --emergency-stop-color: #FF0000; /* Red for emergency stop */
    }
    
    body {
      background: var(--bg-gradient);
      color: var(--text-color);
      min-height: 100vh;
      max-width: 100%;
      overflow-x: hidden;
      padding-bottom: 56px; /* Height of the tabs */
    }
    
    .header {
      padding: 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      background-color: #000000;
      border-bottom: 1px solid #444;
      position: sticky;
      top: 0;
      z-index: 100;
    }
    
    .header h1 {
      font-size: 24px;
      font-weight: 600;
      letter-spacing: 1px;
    }
    
    .icon-button {
      background: none;
      border: none;
      color: white;
      font-size: 24px;
    }
    
    .container {
      padding: 16px;
      max-width: 600px;
      margin: 0 auto;
    }
    
    .btn {
      display: block;
      width: 100%;
      padding: 16px;
      margin: 12px 0;
      border-radius: var(--radius);
      border: none;
      font-weight: 600;
      font-size: 18px;
      color: white;
      text-align: center;
      cursor: pointer;
      transition: all 0.3s ease;
      border: 1px solid #444;
      -webkit-tap-highlight-color: transparent;
    }
    
    .btn-tomato {
      background-color: var(--tomato-color);
    }
    
    .btn-brinjal {
      background-color: var(--brinjal-color);
    }
    
    .btn-chilly {
      background-color: var(--chilly-color);
    }
    
    .btn:hover {
      background-color: #111;
      transform: translateY(-2px);
    }
    
    .btn:disabled {
      background-color: var(--disabled-color);
      color: #888;
      cursor: not-allowed;
      transform: none;
    }
    
    .dropdown {
      background-color: var(--card-bg);
      border-radius: var(--radius);
      padding: 12px;
      margin-bottom: 12px;
      margin-top: -8px;
      color: #333;
      display: none;
      border: 1px solid #ddd;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    }
    
    .dropdown.active {
      display: block;
    }
    
    .dropdown-item {
      display: flex;
      justify-content: space-between;
      padding: 8px 0;
      border-bottom: 1px solid #eee;
    }
    
    .dropdown-item:last-child {
      border-bottom: none;
    }
    
    .status-card {
      background-color: var(--status-bg);
      border-radius: var(--radius);
      padding: 16px;
      margin: 16px 0;
      color: #333;
      border: 1px solid #ddd;
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
    }
    
    .status-card h3 {
      display: flex;
      align-items: center;
      margin-bottom: 8px;
    }
    
    .status-card h3 span {
      margin-left: 8px;
    }
    
    .status-value {
      color: #000000;
      font-weight: 600;
    }
    
    .status-icon {
      display: inline-block;
      width: 20px;
      text-align: center;
    }
    
    .progress-bar {
      height: 12px;
      background-color: #ddd;
      border-radius: 6px;
      margin-top: 8px;
      overflow: hidden;
    }
    
    .progress-fill {
      height: 100%;
      background-color: #000000;
      width: 90%;
    }
    
    .bluetooth-status {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    
    .disconnected {
      color: #000000;
      font-weight: bold;
    }
    
    .connected {
      color: #006400;
      font-weight: bold;
    }
    
    .refresh-btn {
      background-color: #000000;
      color: white;
      border: none;
      border-radius: 4px;
      padding: 8px 16px;
      cursor: pointer;
      transition: background-color 0.2s;
    }
    
    .refresh-btn:hover {
      background-color: #333;
    }
    
    .hire-buttons {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      margin-top: 16px;
    }
    
    .start-btn {
      background-color: var(--start-hire-color);
      color: black;
      border: 1px solid #444;
      font-weight: bold;
    }
    
    .stop-btn {
      background-color: var(--end-hire-color);
      color: black;
      border: 1px solid #444;
      font-weight: bold;
    }
    
    .emergency-btn {
      background-color: var(--emergency-stop-color);
      color: white;
      border: 1px solid #444;
      font-weight: bold;
      margin-top: 16px;
    }
    
    .tabs {
      display: flex;
      border-top: 1px solid #444;
      position: fixed;
      bottom: 0;
      left: 0;
      right: 0;
      background-color: #000;
      z-index: 90;
      height: 56px;
    }
    
    .tab {
      flex: 1;
      text-align: center;
      padding: 16px 8px;
      background-color: rgba(0, 0, 0, 0.2);
      cursor: pointer;
      transition: all 0.2s ease;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    
    .tab.active {
      background-color: rgba(0, 0, 0, 0.5);
      font-weight: bold;
      border-top: 2px solid white;
    }
    
    .page {
      display: none;
    }
    
    .page.active {
      display: block;
    }
    
    .floating-btn {
      position: fixed;
      bottom: 70px;
      right: 16px;
      width: 56px;
      height: 56px;
      border-radius: 28px;
      background-color: white;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
      color: #000000;
      font-size: 24px;
      text-decoration: none;
      z-index: 80;
    }
    
    .alert-card {
      background-color: #f5f5f5;
      color: #000000;
      border-radius: var(--radius);
      padding: 16px;
      margin: 16px 0;
      border-left: 4px solid #000000;
    }
    
    .modal {
      display: none;
      position: fixed;
      z-index: 200;
      left: 0;
      top: 0;
      width: 100%;
      height: 100%;
      background-color: rgba(0,0,0,0.7);
      overflow: auto;
    }
    
    .modal-content {
      background-color: #fefefe;
      margin: 15% auto;
      padding: 20px;
      border-radius: var(--radius);
      width: 80%;
      max-width: 500px;
    }
    
    .modal h2 {
      color: #000;
      margin-bottom: 16px;
    }
    
    .modal p {
      color: #333;
      margin-bottom: 16px;
      font-size: 16px;
    }
    
    .modal-buttons {
      display: flex;
      justify-content: flex-end;
      gap: 12px;
    }
    
    .modal-btn {
      padding: 10px 16px;
      border-radius: 4px;
      font-weight: 600;
      cursor: pointer;
      border: none;
    }
    
    .send-sms-btn {
      background-color: #000;
      color: white;
    }
    
    .cancel-btn {
      background-color: #ddd;
      color: #333;
    }
    
    @media (max-width: 480px) {
      .header h1 {
        font-size: 20px;
      }
      
      .btn {
        font-size: 16px;
        padding: 14px;
      }
      
      .hire-buttons {
        grid-template-columns: 1fr;
      }
      
      .container {
        padding: 12px;
      }
      
      .status-card {
        padding: 12px;
      }
      
      .last-card {
        margin-bottom: 16px;
      }
    }
  </style>
</head>
<body>
  <div class="header">
    <h1>BEHEMOTH</h1>
    <button class="icon-button">📈</button>
  </div>
  
  <div id="home-page" class="page active">
    <div class="container">
      <button class="btn btn-tomato" id="tomato-btn">
        🍅 TOMATO
      </button>
      <div class="dropdown" id="tomato-dropdown">
        <div class="dropdown-item">
          <span>Bed to Bed distance:</span>
          <span>300mm</span>
        </div>
      </div>
      
      <button class="btn btn-brinjal" id="brinjal-btn">
        🍆 BRINJAL
      </button>
      <div class="dropdown" id="brinjal-dropdown">
         <div class="dropdown-item">
          <span>Bed to Bed distance:</span>
          <span>450mm</span>
        </div>
      </div>
      
      <button class="btn btn-chilly" id="chilly-btn">
        🌶️ CHILLY
      </button>
      <div class="dropdown" id="chilly-dropdown">
         <div class="dropdown-item">
          <span>Bed to Bed distance:</span>
          <span>600mm</span>
        </div>
      </div>
      
      <div class="status-card">
        <div class="bluetooth-status">
          <h3>
            <span class="status-icon">📶</span>
            <span>WiFi Status</span>
          </h3>
          <button class="refresh-btn">Refresh</button>
        </div>
        <p id="wifi-status" class="connected">Connecting...</p>
      </div>
      
      <div class="hire-buttons">
        <button class="btn start-btn" id="start-hire">Start Hire</button>
        <button class="btn stop-btn" id="end-hire">End Hire</button>
      </div>
    </div>
  </div>
  
  <div id="monitoring-page" class="page">
    <div class="container">
      <div class="status-card">
        <h3>Plants Transplanted</h3>
        <div class="status-value" id="plant-counter">23</div>
      </div>
      
      <div class="status-card">
        <h3>Water Level</h3>
        <div class="status-value" id="water-level">75%</div>
        <div class="progress-bar">
          <div class="progress-fill" id="water-level-fill" style="width: 75%"></div>
        </div>
      </div>
      
      <div class="alert-card" style="display: none;">
  <h3>⚠️ Tray End Alert</h3>
  <p>Current tray is almost empty. Please prepare next tray for seamless operation.</p>
</div>
      
      <button class="btn emergency-btn" id="emergency-stop">
        EMERGENCY STOP
      </button>
    </div>
  </div>
  
  <!-- Modal for hire summary -->
  <div id="hire-summary-modal" class="modal">
    <div class="modal-content">
      <h2>Hire Summary</h2>
      <p id="hire-duration"></p>
      <p id="hire-cost"></p>
      <div class="modal-buttons">
        <button class="modal-btn cancel-btn" id="cancel-sms">Cancel</button>
        <button class="modal-btn send-sms-btn" id="send-sms">Send SMS</button>
      </div>
    </div>
  </div>
  
  <div class="tabs">
    <div class="tab active" data-page="home-page">Home</div>
    <div class="tab" data-page="monitoring-page">Monitoring</div>
  </div>

  <script>
    // DOM Elements
    const tomatoBtn = document.getElementById('tomato-btn');
    const tomatoDropdown = document.getElementById('tomato-dropdown');
    const brinjalBtn = document.getElementById('brinjal-btn');
    const brinjalDropdown = document.getElementById('brinjal-dropdown');
    const chillyBtn = document.getElementById('chilly-btn');
    const chillyDropdown = document.getElementById('chilly-dropdown');
    const startHireBtn = document.getElementById('start-hire');
    const endHireBtn = document.getElementById('end-hire');
    const tabs = document.querySelectorAll('.tab');
    const pages = document.querySelectorAll('.page');
    const refreshBtn = document.querySelector('.refresh-btn');
    const wifiStatus = document.getElementById('wifi-status');
    const waterLevelElement = document.getElementById('water-level');
    const waterLevelFill = document.getElementById('water-level-fill');
    const plantCounterElement = document.getElementById('plant-counter');
    const emergencyStopBtn = document.getElementById('emergency-stop');
    const hireSummaryModal = document.getElementById('hire-summary-modal');
    const hireDurationElement = document.getElementById('hire-duration');
    const hireCostElement = document.getElementById('hire-cost');
    const sendSmsBtn = document.getElementById('send-sms');
    const cancelSmsBtn = document.getElementById('cancel-sms');
    
    // Variables
    let plantCount = 23;
    let waterLevel = 75;
    let hiringData = {
      isHired: false,
      startTime: 0,
      duration: 0,
      cost: 0
    };
    const MINUTE_RATE = 1.0; // Rupees per minute
    
    // Dropdown toggle function
    function toggleDropdown(dropdown) {
      dropdown.classList.toggle('active');
    }
    
    // Event listeners for dropdown buttons
    tomatoBtn.addEventListener('click', () => toggleDropdown(tomatoDropdown));
    brinjalBtn.addEventListener('click', () => toggleDropdown(brinjalDropdown));
    chillyBtn.addEventListener('click', () => toggleDropdown(chillyDropdown));
    
    // Tab navigation
    tabs.forEach(tab => {
      tab.addEventListener('click', () => {
        tabs.forEach(t => t.classList.remove('active'));
        tab.classList.add('active');
        
        const pageId = tab.dataset.page;
        pages.forEach(page => page.classList.remove('active'));
        document.getElementById(pageId).classList.add('active');
      });
    });
    
    // Check hire status from server on page load
    async function checkHireStatus() {
      try {
        const response = await fetch('/hire-status');
        const data = await response.json();
        
        hiringData.isHired = data.isHired;
        hiringData.startTime = data.startTime;
        hiringData.elapsedSeconds = data.elapsedSeconds;
        
        // Update UI based on hire status
        updateHireButtonsState();
      } catch (error) {
        console.error('Error fetching hire status:', error);
      }
    }
    
   // Update WiFi status to show if any device is connected
function checkWifiStatus() {
  fetch('/wifi-status')
    .then(response => response.json())
    .then(data => {
      if (data.apMode) {
        if (data.deviceConnected) {
          wifiStatus.textContent = 'Phone connected to BEHEMOTH_AP';
          wifiStatus.className = 'connected';
        } else {
          wifiStatus.textContent = 'AP Mode - No device connected';
          wifiStatus.className = 'disconnected';
        }
      } else {
        wifiStatus.textContent = 'Error in AP mode';
        wifiStatus.className = 'disconnected';
      }
    })
    .catch(error => {
      console.error('Error checking WiFi status:', error);
      wifiStatus.textContent = 'Error checking status';
      wifiStatus.className = 'disconnected';
    });
}
    // WiFi refresh button
    refreshBtn.addEventListener('click', () => {
      wifiStatus.textContent = 'Checking...';
      checkWifiStatus();
    });
    
    // Start Hire
    startHireBtn.addEventListener('click', async () => {
      try {
        const response = await fetch('/start-hire', {
          method: 'POST'
        });
        
        const data = await response.json();
        if (data.success) {
          hiringData.isHired = true;
          hiringData.startTime = data.startTime;
          updateHireButtonsState();
          alert('Hire started successfully');
        } else {
          alert('Error starting hire: ' + data.message);
        }
      } catch (error) {
        console.error('Error starting hire:', error);
        alert('Error starting hire. Please try again.');
      }
    });
    
    // End Hire
    endHireBtn.addEventListener('click', async () => {
      if (!hiringData.isHired) {
        alert('No active hire to end.');
        return;
      }
      
      try {
        const response = await fetch('/end-hire', {
          method: 'POST'
        });
        
        const data = await response.json();
        if (data.success) {
          hiringData.isHired = false;
          hiringData.duration = data.duration;
          hiringData.cost = data.cost;
          
          // Format duration for display, including hours, minutes, and seconds
          const hours = Math.floor(data.duration / 3600);
          const minutes = Math.floor((data.duration % 3600) / 60);
          const seconds = Math.floor(data.duration % 60);
          
          // Create duration text with hours, minutes, and seconds
          let durationText = '';
          if (hours > 0) {
            durationText += `${hours} hour${hours !== 1 ? 's' : ''} `;
          }
          if (minutes > 0 || hours > 0) {
            durationText += `${minutes} minute${minutes !== 1 ? 's' : ''} `;
          }
          durationText += `${seconds} second${seconds !== 1 ? 's' : ''}`;
          
          // Update modal content
          hireDurationElement.textContent = `Duration: ${durationText}`;
          hireCostElement.textContent = `Cost: ₹${data.cost.toFixed(2)}`;
          
          // Show the modal
          hireSummaryModal.style.display = 'block';
          
          // Update UI
          updateHireButtonsState();
        } else {
          alert('Error ending hire: ' + data.message);
        }
      } catch (error) {
        console.error('Error ending hire:', error);
        alert('Error ending hire. Please try again.');
      }
    });
    
    // Emergency Stop Button
    emergencyStopBtn.addEventListener('click', async () => {
      if (confirm('Are you sure you want to activate EMERGENCY STOP? This will shut down all systems immediately!')) {
        try {
          const response = await fetch('/emergency-stop', {
            method: 'POST'
          });
          
          const data = await response.json();
          if (data.success) {
            emergencyStopBtn.textContent = 'EMERGENCY STOP ACTIVATED';
            emergencyStopBtn.style.backgroundColor = '#990000'; // Darker red
            alert('Emergency stop activated! All systems powered down.');
          } else {
            alert('Error activating emergency stop: ' + data.message);
          }
        } catch (error) {
          console.error('Error activating emergency stop:', error);
          alert('Error activating emergency stop. Please try again.');
        }
      }
    });
    
    // Send SMS button
    sendSmsBtn.addEventListener('click', () => {
      // Format duration for SMS
      const hours = Math.floor(hiringData.duration / 3600);
      const minutes = Math.floor((hiringData.duration % 3600) / 60);
      const seconds = Math.floor(hiringData.duration % 60);
      
      let durationText = '';
      if (hours > 0) {
        durationText += `${hours} hour${hours !== 1 ? 's' : ''} `;
      }
      if (minutes > 0 || hours > 0) {
        durationText += `${minutes} minute${minutes !== 1 ? 's' : ''} `;
      }
      durationText += `${seconds} second${seconds !== 1 ? 's' : ''}`;
      
      const message = `BEHEMOTH Hire Summary: Duration: ${durationText}, Cost: ₹${hiringData.cost.toFixed(2)}`;
      const phoneNumber = '919847016556';
      
      // Close the modal
      hireSummaryModal.style.display = 'none';
      
      // Open SMS app with prefilled message
      window.location.href = `sms:${phoneNumber}?body=${encodeURIComponent(message)}`;
    });
    
    // Cancel SMS button
    cancelSmsBtn.addEventListener('click', () => {
      hireSummaryModal.style.display = 'none';
    });
    
    // Update hire buttons state based on current status
    function updateHireButtonsState() {
      if (hiringData.isHired) {
        startHireBtn.disabled = true;
        startHireBtn.style.opacity = '0.5';
        endHireBtn.disabled = false;
        endHireBtn.style.opacity = '1';
      } else {
        startHireBtn.disabled = false;
        startHireBtn.style.opacity = '1';
        endHireBtn.disabled = true;
        endHireBtn.style.opacity = '0.5';
      }
    }
    
    // Update water level simulation
    function updateWaterLevel() {
      if (Math.random() > 0.8) {
        waterLevel = Math.max(0, Math.min(100, waterLevel - Math.floor(Math.random() * 3)));
        waterLevelElement.textContent = `${waterLevel}%`;
        waterLevelFill.style.width = `${waterLevel}%`;
      }
      
      setTimeout(updateWaterLevel, 10000);
    }
    
    // Update plant counter simulation
    function updatePlantCounter() {
      if (Math.random() > 0.7) {
        plantCount++;
        plantCounterElement.textContent = plantCount;
      }
      
      setTimeout(updatePlantCounter, 7000);
    }
    
    // Initialize the system
function initSystem() {
  // Check initial hire status
  checkHireStatus();
  
  // Check initial WiFi status
  checkWifiStatus();
  
  // Start periodic sensor data updates
  updateSensorData();
}

// Fetch sensor data from ESP32
function updateSensorData() {
  fetch('/sensor-data')
    .then(response => response.json())
    .then(data => {
     
      
      // Update tray end alert visibility
      const alertCard = document.querySelector('.alert-card');
      if (data.trayEndAlert) {
        alertCard.style.display = 'block';
      } else {
        alertCard.style.display = 'none';
      }
    })
    .catch(error => {
      console.error('Error fetching sensor data:', error);
    });
  
  // Schedule next update
  setTimeout(updateSensorData, 3000); // Update every 3 seconds
}
    
    // Initialize on page load
    window.addEventListener('load', initSystem);
  </script>
</body>
</html>)rawliteral";

// Function to update elapsed time in persistent storage
void updateElapsedTime() {
  if (isHired) {
    unsigned long currentTime = millis() / 1000;
    unsigned long elapsedSeconds = currentTime - startHireTime;
    
    // Store the elapsed time in persistent storage
    preferences.putULong("elapsedSeconds", elapsedSeconds);
  }
}
// Motion Control System - Functions
void handleModeSwitch() {
  bool currentBtnState = digitalRead(joystickBtnPin);
  if (lastBtnState == HIGH && currentBtnState == LOW) {
    manualMode = !manualMode;
    Serial.println(manualMode ? "Manual Mode ON" : "Sensor Mode ON");
    delay(300);  // Debounce
  }
  lastBtnState = currentBtnState;
}

void handleJoystickControl() {
  int xVal = analogRead(xAxisPin);
  int yVal = analogRead(yAxisPin);

  const int maxThreshold = 3800;
  const int minThreshold = 300;

  // X-axis controls Stepper 1 (left/right)
  if (xVal < minThreshold) {
    rotateStepper(500, false); // Fully left
  } else if (xVal > maxThreshold) {
    rotateStepper(500, true);  // Fully right
  }

  // Y-axis controls Stepper 2 (up/down)
  if (yVal < minThreshold) {
    rotateStepper2(500, true); // Fully up
  } else if (yVal > maxThreshold) {
    rotateStepper2(500, false); // Fully down
  }
}

void handleSensorControl() {
  unsigned long currentTime = millis();

  if (digitalRead(pirSensor1) == LOW && activeSensor == 1 && (currentTime - lastTriggerTime > debounceDelay)) {
    rotateServos(0);
    servosRotated = true;
    activeSensor = 2;
    lastTriggerTime = currentTime;
  }

  if (digitalRead(pirSensor2) == LOW && activeSensor == 2 && (currentTime - lastTriggerTime > debounceDelay)) {
    rotateServos(90);
    activeSensor = 1;
    lastTriggerTime = currentTime;

    if (servosRotated) {
      rotateStepper(stepsPerRevolution, true);
      servosRotated = false;
    }

    sensor2TriggerCount++;
    if (sensor2TriggerCount >= 6) {
      rotateSecondStepper = true;
      sensor2TriggerCount = 0;
      toggleFirstStepperDirection();
    }

    if (rotateSecondStepper) {
      rotateStepper2(stepsPerRevolution, true);
      rotateSecondStepper = false;
    }
  }
}

void rotateStepper(int steps, bool forward) {
  digitalWrite(DIR_PIN, forward ? HIGH : LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(pulseHighTime);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(pulseLowTime);
  }
}

void rotateStepper2(int steps, bool forward) {
  digitalWrite(DIR_PIN_2, forward ? HIGH : LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(PUL_PIN_2, HIGH);
    delayMicroseconds(pulseHighTime);
    digitalWrite(PUL_PIN_2, LOW);
    delayMicroseconds(pulseLowTime);
  }
}

void toggleFirstStepperDirection() {
  static bool direction = true;
  direction = !direction;
  digitalWrite(DIR_PIN, direction ? HIGH : LOW);
  Serial.println("First stepper motor direction toggled.");
}

void rotateServos(int angle) {
  servo1.write(angle);
  servo2.write(angle);
}

// Function to handle plant count sensor
void handlePlantCountSensor() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(plantCountSensor);
  
  // Detect falling edge (object detected)
  if (lastState == HIGH && currentState == LOW) {
    plantCount++;
    
    // Update plant counter via WebSocket if needed
    // For now, the updates will be picked up on the next page refresh
    
    Serial.print("Plant count increased: ");
    Serial.println(plantCount);
    
    delay(50); // Debounce
  }
  
  lastState = currentState;
}

// Function to handle water level sensor
void handleWaterLevelSensor() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(waterLevelSensor);
  static unsigned long lastUpdateTime = 0;
  
  // Only update water level occasionally to prevent rapid fluctuations
  if (millis() - lastUpdateTime > 5000) { // 5 second minimum between updates
    // Detect falling edge (object detected)
    if (lastState == HIGH && currentState == LOW) {
      // Adjust water level (assume each detection decreases water by 5%)
      waterLevel = max(0, waterLevel - 5);
      
      Serial.print("Water level updated: ");
      Serial.println(waterLevel);
      
      lastUpdateTime = millis();
    }
    
    lastState = currentState;
  }
}

// Function to handle tray end sensor
void handleTrayEndSensor() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(trayEndSensor);
  
  // Detect falling edge (object detected)
  if (lastState == HIGH && currentState == LOW) {
    trayTriggerCount++;
    
    Serial.print("Tray sensor triggered: ");
    Serial.print(trayTriggerCount);
    Serial.print("/91 - Alert active: ");
    
    // Check if we've reached 91 triggers
    if (trayTriggerCount >= 91) {
      trayEndAlertActive = true;
      Serial.println("YES");
    } else {
      Serial.println("NO");
    }
    
    delay(50); // Debounce
  }
  
  lastState = currentState;
}

// Function to add new routes for sensor data
void addSensorDataRoutes() {
  // Route to get sensor data (plant count, water level, tray alert)
  server.on("/sensor-data", HTTP_GET, [](AsyncWebServerRequest *request){
    String response = "{\"plantCount\":" + String(plantCount) + 
                     ",\"waterLevel\":" + String(waterLevel) + 
                     ",\"trayEndAlert\":" + String(trayEndAlertActive ? "true" : "false") + "}";
    request->send(200, "application/json", response);
  });
}


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  // Add this inside your existing setup() function
  servo1.attach(4);
  servo2.attach(5);
  // Initialize proximity sensors
  pinMode(plantCountSensor, INPUT);
  pinMode(waterLevelSensor, INPUT);
  pinMode(trayEndSensor, INPUT);
  pinMode(pirSensor1, INPUT);
  pinMode(pirSensor2, INPUT);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PUL_PIN_2, OUTPUT);  
  pinMode(DIR_PIN_2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(joystickBtnPin, INPUT_PULLUP);
  
  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(DIR_PIN_2, HIGH);
  
  servo1.write(90);
  servo2.write(90);
  // Initialize emergency stop pin
  pinMode(emergencyStopPin, OUTPUT);
  digitalWrite(emergencyStopPin, LOW); // Start with relay off (normal operation)
  
  // Initialize preferences for persistent storage
  preferences.begin("behemoth", false);  // false = RW mode
  
  // Record boot time as seconds since ESP started
  bootTime = millis() / 1000;
  
  // Load hire status from persistent storage
  isHired = preferences.getBool("isHired", false);
  unsigned long elapsedSeconds = preferences.getULong("elapsedSeconds", 0);
  
  // If there's an active hire, calculate the relative start time based on
  // the stored elapsed time and current boot time
  if (isHired) {
    // Calculate startHireTime relative to current boot session
    startHireTime = bootTime - elapsedSeconds;
    
    // Update elapsed seconds in case of reboot during hire
    updateElapsedTime();
    
    Serial.println("Restored active hire:");
    Serial.print("Boot time (s): ");
    Serial.println(bootTime);
    Serial.print("Elapsed seconds: ");
    Serial.println(elapsedSeconds);
    Serial.print("Calculated start time: ");
    Serial.println(startHireTime);
  }
  
  // Set up WiFi in Access Point mode
  WiFi.softAP(ssid, password);
  
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });
  
  // Route to get WiFi status - checks if any device is connected
server.on("/wifi-status", HTTP_GET, [](AsyncWebServerRequest *request){
  int stationCount = WiFi.softAPgetStationNum();
  bool deviceConnected = (stationCount > 0);
  
  String response = "{\"apMode\":true,\"deviceConnected\":" + String(deviceConnected ? "true" : "false") + 
                   ",\"apName\":\"" + String(ssid) + "\"}";
  request->send(200, "application/json", response);
});
  
  // Route to get hire status
  server.on("/hire-status", HTTP_GET, [](AsyncWebServerRequest *request){
    // Calculate elapsed time if hired
    unsigned long elapsedSeconds = 0;
    if (isHired) {
      elapsedSeconds = (millis() / 1000) - startHireTime;
    }
    
    String response = "{\"isHired\":" + String(isHired ? "true" : "false") + 
                     ",\"startTime\":" + String(startHireTime) + 
                     ",\"elapsedSeconds\":" + String(elapsedSeconds) + "}";
    request->send(200, "application/json", response);
  });
  
  // Route to start hire
  server.on("/start-hire", HTTP_POST, [](AsyncWebServerRequest *request){
    if (isHired) {
      request->send(400, "application/json", "{\"success\":false,\"message\":\"Already hired\"}");
      return;
    }
    
    // Get current time
    startHireTime = millis() / 1000;  // Convert to seconds
    isHired = true;
    
    // Save hire status to persistent storage
    preferences.putBool("isHired", true);
    preferences.putULong("elapsedSeconds", 0);  // Reset elapsed time
    
    String response = "{\"success\":true,\"startTime\":" + String(startHireTime) + "}";
    request->send(200, "application/json", response);
  });
  
  // Route to end hire
  server.on("/end-hire", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!isHired) {
      request->send(400, "application/json", "{\"success\":false,\"message\":\"No active hire\"}");
      return;
    }
    
    // Calculate duration and cost
    unsigned long currentTime = millis() / 1000;
    unsigned long duration = currentTime - startHireTime;
    
    // Calculate cost based on duration (per minute)
    float cost = (duration / 60.0) * HOURLY_RATE;
    
    // Reset hire status
    isHired = false;
    preferences.putBool("isHired", false);
    
    // Create JSON response with duration and cost
    String response = "{\"success\":true,\"duration\":" + String(duration) + 
                     ",\"cost\":" + String(cost) + "}";
    
    request->send(200, "application/json", response);
  });
  
  // Route for emergency stop
  server.on("/emergency-stop", HTTP_POST, [](AsyncWebServerRequest *request){
    // Activate emergency stop relay
    digitalWrite(emergencyStopPin, HIGH); // Turn off all electronics
    
    emergencyStopActive = true;
    
    // Also end any active hire
    if (isHired) {
      isHired = false;
      preferences.putBool("isHired", false);
    }
    
    request->send(200, "application/json", "{\"success\":true}");
  });
  // Add routes for sensor data
  addSensorDataRoutes();
  // Start server
  server.begin();
}

void loop() {
  // Motion control system
  handleModeSwitch();
  
  if (manualMode) {
    handleJoystickControl();
  } else {
    handleSensorControl();
  }
  
  // Handle the new sensors
  handlePlantCountSensor();
  handleWaterLevelSensor();
  handleTrayEndSensor();
  
  // Update elapsed time periodically to handle potential reboots
  if (isHired && (millis() % 30000 == 0)) {  // Every 30 seconds
    updateElapsedTime();
  }
}