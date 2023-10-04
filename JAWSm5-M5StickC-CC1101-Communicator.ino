#include <M5StickC.h>
#include <SPI.h>
#include <CC1101.h>

#define CS_PIN 10

CC1101 radio;

int menuIndex = 0;
const char* menuOptions[] = {"Transmit Data", "Receive Data", "Change Frequency"};
int currentFrequencyIndex = 0;
const int numFrequencies = 3;
float frequencies[numFrequencies] = {433.92, 868.0, 915.0}; // Add your frequencies here

void setup() {
  M5.begin();
  M5.Lcd.setRotation(1);
  Serial.begin(9600);

  radio.begin();
  setFrequency(frequencies[currentFrequencyIndex]);

  radio.setTxPower(10);

  M5.Lcd.fillScreen(BLACK);
  displayMenu();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    menuIndex = (menuIndex - 1 + 3) % 3;
    displayMenu();
  }

  if (M5.BtnB.wasPressed()) {
    if (menuIndex == 0) {
      transmitData();
    } else if (menuIndex == 1) {
      receiveData();
    } else if (menuIndex == 2) {
      changeFrequency();
    }
  }

  if (M5.BtnC.wasPressed()) {
    menuIndex = (menuIndex + 1) % 3;
    displayMenu();
  }
}

void displayMenu() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);

  for (int i = 0; i < 3; i++) {
    if (i == menuIndex) {
      M5.Lcd.setTextColor(GREEN);
    } else {
      M5.Lcd.setTextColor(WHITE);
    }

    M5.Lcd.setCursor(10, 20 + i * 40);
    M5.Lcd.printf("%s", menuOptions[i]);
  }
}

void transmitData() {
  uint8_t packet[] = {0x01, 0x02, 0x03};
  radio.sendData(packet, sizeof(packet));
  Serial.println("Data transmitted.");
}

void receiveData() {
  if (radio.receiveData()) {
    uint8_t rxBuffer[radio.getPacketLength()];
    radio.getReceivedData(rxBuffer, radio.getPacketLength());

    Serial.println("Received Packet:");
    for (int i = 0; i < radio.getPacketLength(); i++) {
      Serial.print(rxBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void changeFrequency() {
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(10, 20);
  M5.Lcd.printf("Select Frequency:");

  while (true) {
    if (M5.BtnA.wasPressed()) {
      currentFrequencyIndex = (currentFrequencyIndex - 1 + numFrequencies) % numFrequencies;
    }
    if (M5.BtnB.wasPressed()) {
      setFrequency(frequencies[currentFrequencyIndex]);
      break;
    }
    if (M5.BtnC.wasPressed()) {
      break;
    }

    M5.Lcd.setCursor(10, 60);
    M5.Lcd.printf("%.2f MHz", frequencies[currentFrequencyIndex]);
    delay(100);
  }

  displayMenu();
}

void setFrequency(float freq) {
  radio.setFrequency(freq);
}
