#include <SPI.h>
#include <LoRa.h>

const int irSensorPin = 3;  // Pin connected to the IR sensor
volatile unsigned long pulseCount = 0;  // To store the number of energy meter pulses

const int csPin = 10;     // LoRa radio chip select (NSS)
const int resetPin = 9;   // LoRa radio reset (RST)
const int dio0Pin = 8;    // LoRa radio DIO0 (interrupt)

// Pin for battery measurement
const int batteryPin = A0;  // Analog pin for battery voltage measurement
const float lowBatteryThreshold = 3.3;  // Threshold for low battery warning

// Interrupt service routine (ISR) to count pulses from the energy meter
void IRpulse() {
  pulseCount++;  // Increment pulse count when a pulse is detected
}

// Function to read battery voltage using a voltage divider with two 10k resistors
float readBatteryVoltage() {
  int sensorValue = analogRead(batteryPin);  // Read the analog value (0-1023)
  float voltage = sensorValue * (5.0 / 1023.0) * 2.0;  // Calculate actual voltage
  return voltage;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for the serial connection

  // Initialize the IR sensor pin as input and attach interrupt
  pinMode(irSensorPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), IRpulse, RISING);  // Attach interrupt on rising edge of the IR pulse

  // Initialize LoRa and set the pins
  LoRa.setPins(csPin, resetPin, dio0Pin);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa and IR Sensor Initialized");
}

void loop() {
  static unsigned long lastSendTime = 0;
  static unsigned long lastBatteryCheckTime = 0;
  unsigned long currentTime = millis();

  // Send energy meter data every 10 seconds
  if (currentTime - lastSendTime > 10000) {
    lastSendTime = currentTime;

    // Copy the pulse count to a local variable and reset it
    unsigned long currentPulseCount = pulseCount;
    pulseCount = 0;  // Reset the pulse count for the next interval

    Serial.print("Energy Meter Pulse Count: ");
    Serial.println(currentPulseCount);

    // Send the pulse count over LoRa
    LoRa.beginPacket();
    LoRa.print("Pulse Count: ");  // Send the message
    LoRa.print(currentPulseCount);  // Send the pulse count
    LoRa.endPacket();
  }

  // Check battery level every 30 seconds
  if (currentTime - lastBatteryCheckTime > 30000) {
    lastBatteryCheckTime = currentTime;

    // Read the battery voltage
    float batteryVoltage = readBatteryVoltage();
    Serial.print("Battery Voltage: ");
    Serial.println(batteryVoltage);

    // Send low battery warning if voltage is below the threshold
    if (batteryVoltage < lowBatteryThreshold) {
      Serial.println("Low battery warning!");

      LoRa.beginPacket();
      LoRa.print("Warning: Low Battery! Voltage: ");
      LoRa.print(batteryVoltage);
      LoRa.endPacket();
    }
  }
}
