#include <SoftwareSerial.h>

// Define OBIS codes for various parameters
const String OBIS_VOLTAGE = "1.0.1.8.0.255"; // Voltage OBIS code
const String OBIS_CURRENT = "1.0.2.8.0.255"; // Current OBIS code
const String OBIS_POWER = "1.0.3.8.0.255";   // Power OBIS code
const String OBIS_ENERGY = "1.8.0";          // Energy OBIS code

int rxPin = 9;
int txPin = 8;
// Set up the software serial for RS-485 communication
SoftwareSerial rs485(rxPin, txPin); // RX, TX

void setup() {
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    Serial.begin(9600);
    rs485.begin(9600);
    Serial.println("DLMS Client Ready");
}

void loop() {
    delay(1000);
    // Request data every 5 seconds
    requestData(OBIS_ENERGY);
    readResponse(); // Request energy data
    delay(1000);
    
    requestData(OBIS_VOLTAGE);
    readResponse(); // Request voltage data
    delay(1000);
    
    requestData(OBIS_CURRENT);
    readResponse(); // Request current data
    delay(1000);
    
    requestData(OBIS_POWER);
    readResponse(); // Request power data
    delay(1000);
    
    // Read and display response from the energy meter
}

void requestData(String obisCode) {
    String request = "<06>" + obisCode + "<13><10>"; // Construct DLMS request
    rs485.print(request); // Send request via RS-485
    Serial.print("Request sent: ");
    Serial.println(request); // Print the request to the serial monitor
}

void readResponse() {
  while(1){
    if (Serial.available()) {
        String response = "";
        while (Serial.available()) {
            char c = Serial.read();
            response += c;
            delay(10); // Small delay to allow complete message reception
        }
        Serial.print("Response received: ");
        Serial.println(response); // Print the response to the serial monitor
        return;
    }
  }
}