#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);   // CE, CSN
const byte address[6] = "00001";

struct DataPacket {
  int x;
  int y;
};

DataPacket data;

const int MPU_ADDR = 0x68;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("TX boot");

  Wire.begin();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  if (!radio.begin()) {
    Serial.println("radio.begin failed");
    while (1);
  }

  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("TX ready");
}

void loop() {
  int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  data.x = map(AcX, -17000, 17000, 0, 180);
  data.y = map(AcY, -17000, 17000, 180, 0);   // inverted Y

  data.x = constrain(data.x, 0, 180);
  data.y = constrain(data.y, 0, 180);

  bool ok = radio.write(&data, sizeof(data));

  Serial.print("X: ");
  Serial.print(data.x);
  Serial.print("  Y: ");
  Serial.print(data.y);
  Serial.print("  | ");
  Serial.println(ok ? "SENT" : "FAILED");

  delay(50);
}