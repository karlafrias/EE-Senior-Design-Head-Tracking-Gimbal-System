#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10);   // CE, CSN
const byte address[6] = "00001";

Servo pan;
Servo tilt;

struct DataPacket {
  int x;
  int y;
};

DataPacket data;

int panPos = 90;
int tiltPos = 90;

const int deadband = 2;   
const float alpha = 0.2; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("RX boot");

  pan.attach(3);    // D3 = pan
  tilt.attach(5);   // D5 = tilt

  pan.write(panPos);
  tilt.write(tiltPos);

  if (!radio.begin()) {
    Serial.println("radio.begin failed");
    while (1);
  }

  radio.setAutoAck(false);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(100);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("RX ready");
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));

    data.x = constrain(data.x, 0, 180);
    data.y = constrain(data.y, 0, 180);

    if (abs(data.x - panPos) > deadband) {
      panPos = panPos + alpha * (data.x - panPos);
    }

    if (abs(data.y - tiltPos) > deadband) {
      tiltPos = tiltPos + alpha * (data.y - tiltPos);
    }

    pan.write(panPos);
    tilt.write(tiltPos);

    Serial.print("Pan: ");
    Serial.print(panPos);
    Serial.print("  Tilt: ");
    Serial.println(tiltPos);
  }

  delay(15);
}