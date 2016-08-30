#include <TM1637Display.h>

#define CLK 4
#define DIO 3
TM1637Display display(CLK, DIO);

static const int bufferSize = 100;

int buffer[bufferSize];
int bufferIndex;
int bufferSum;
int previousSensorValue;
int previousSign;

void initializeBuffer() {
  bufferIndex = 0;
  bufferSum = 0;
  for (int i=0; i<bufferSize; i++) {
    buffer[i] = 0;
  }
}

void insertIntoBuffer(int value) {
  bufferSum -= buffer[bufferIndex];
  bufferSum += value;
  buffer[bufferIndex] = value;
  bufferIndex = (bufferIndex + 1) % bufferSize;
}

int bufferAverage() {
  return bufferSum / bufferSize;
}

void setup() {
  display.setBrightness(0x08);
  initializeBuffer();
  previousSensorValue = 0;
  previousSign = 0;
}

int readSensor() {
}

int signChange() {
}

void loop() {
  int rawSensor = readSensor();
  insertIntoBuffer(rawSensor);
  int sensorValue = bufferAverage();
  int diff = sensorValue - previousSensorValue;
  int sign;

  if (diff >= 0) {
    sign = 1;
  } else {
    sign = -1;
  }

  if (sign * previousSign < 0) {
    signChange();
  }

  int previousSensorValue = sensorValue;
  int previousSign = sign;
}
