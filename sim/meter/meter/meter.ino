#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

int rxPin = 9;
int txPin = 10;
// Set up the software serial for RS-485 communication
SoftwareSerial rs485(rxPin, txPin); // RX, TX

// Initialize the LCD (adjust pins as necessary)
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float voltage = 230.0; // Simulated voltage
float current = 0.0;   // Simulated current
float power = 0.0;     // Simulated power
float energy = 0.0;    // Simulated energy consumed

// Define OBIS codes for various parameters
const String OBIS_VOLTAGE = "1.0.1.8.0.255"; // Voltage OBIS code
const String OBIS_CURRENT = "1.0.2.8.0.255"; // Current OBIS code
const String OBIS_POWER = "1.0.3.8.0.255";   // Power OBIS code
const String OBIS_ENERGY = "1.8.0";          // Energy OBIS code


void setup() {
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    Serial.begin(9600);
    rs485.begin(9600);
    lcd.begin(16, 2);
    lcd.print("DLMS Energy Meter");
    delay(500);
    lcd.clear();
}

void loop() {

  current = analogRead(A0) * (5.0 / 1024.0); // Read from analog pin A0
  power = voltage * current; // Calculate power in Watts
  energy += power / 3600;

    //Check if data is available on RS-485
    if (Serial.available()) {
        Serial.println("Recieved");
        String request = readDLMSRequest();
        Serial.println(request);
         // Read the incoming DLMS request  
        if (isValidDLMSRequest(request)) { // Validate the request format
            processDLMSRequest(request); // Send data in response to the request
        }
    }
    delay(100);
    updateLCD();
}

String readDLMSRequest() {
    String request = "";
    while (Serial.available()) {
        char c = Serial.read();
        request += c;
        delay(10); // Small delay to allow complete message reception
    }
    return request;
}


bool isValidDLMSRequest(String request) {
    return request.startsWith("<06>") && request.endsWith("<13><10>"); 
}

void processDLMSRequest(String request) {
    if (request == "<06>" + OBIS_ENERGY + "<13><10>") {
       sendDLMSData(OBIS_ENERGY, energy); // Send energy data in response to the request
    } else if (request == "<06>" + OBIS_VOLTAGE + "<13><10>") {
        sendDLMSData(OBIS_VOLTAGE, voltage); // Send voltage data
    } else if (request == "<06>" + OBIS_CURRENT + "<13><10>") {
        sendDLMSData(OBIS_CURRENT, current); // Send current data
    } else if (request == OBIS_POWER) {
        sendDLMSData(OBIS_POWER, power); // Send power data
    }
     // Update LCD display with current values
}


void sendDLMSData(String obisCode, float value) {
    String unit;
    if (obisCode == OBIS_ENERGY){
      unit = "kWh";
    }
    else if (obisCode == OBIS_VOLTAGE){
      unit = "V";
    }
    else if (obisCode == OBIS_CURRENT){
      unit = "A";
    }
    else if (obisCode == OBIS_POWER){
      unit = "W";
    }
    rs485.print("<06>"); // Start of frame
    rs485.print(obisCode + "(" + value + " " + unit + ")<13><10>"); // Send data with OBIS code
}

void updateLCD() {
    lcd.setCursor(1, 0);
    lcd.print("Power: ");
    lcd.print(power);
    lcd.print(" W");
    
    lcd.setCursor(1, 1);
    lcd.print("Energy: ");
    lcd.print(energy);
    lcd.print(" kWh");

    delay(1000);
    lcd.clear();

    lcd.setCursor(1, 0);
    lcd.print("Voltage: ");
    lcd.print(voltage);
    lcd.print(" V");
    
    lcd.setCursor(1, 1);
    lcd.print("Current: ");
    lcd.print(current);
    lcd.print(" A");

    delay(1000);
    lcd.clear();

}