#include <TM1637Display.h>

#define CLK 4
#define DIO 3
TM1637Display display(CLK, DIO);

class Buffer {
  public:
    Buffer(unsigned int);
    void insert(int value);
    int getAverage();
    int getSize();

  private:
    unsigned int size;
    int *buffer;
    int index;
    long sum;
};

Buffer::Buffer(unsigned int n) {
  size = n;
  index = 0;
  sum = 0;
  buffer = (int *) malloc(size * sizeof(int));

  for (int i=0; i<size; i++) {
    buffer[i] = 0;
  }
}

void Buffer::insert(int value) {
  sum -= buffer[index];
  sum += value;
  buffer[index] = value;
  index = (index + 1) % size;
}

int Buffer::getAverage() {
  return sum / size;
}

int Buffer::getSize() {
  return size;
}

typedef unsigned long Millis;

const int flexPin = A0;
const int flexThreshold = 20;
const Millis millisPerMinute = 60000;
const int sensorBufferSize = 32;
const int perMinuteBufferSize = 4;

Buffer sensorBuffer(sensorBufferSize);
Buffer perMinuteBuffer(perMinuteBufferSize);
int baseValue;
boolean aboveThreshold;
Millis lastTickTime;

int readSensor() {
  int rawSensor =  analogRead(flexPin);
  sensorBuffer.insert(rawSensor);
  return rawSensor;
}

void calibrateSensor() {
  for (int i=0; i<sensorBuffer.getSize(); i++) {
    readSensor();
  }
  baseValue = sensorBuffer.getAverage();
}

void setup() {
  Serial.begin(9600);

  display.setBrightness(0x08);
  display.showNumberDec(0, false, 4, 0);

  pinMode(flexPin, INPUT);

  calibrateSensor();
  aboveThreshold = false;
}

int tick() {
}

boolean isTick(int sensorValue) {
  int diff = sensorValue - baseValue;

  if (diff > flexThreshold) {
    if (!aboveThreshold) {
      aboveThreshold = true;
      return true;
    }
  } else {
    aboveThreshold = false;
  }
  return false;
}

void displayCadence(int cadence) {
  display.showNumberDec(cadence, false, 4, 0);
}

void loop() {
  int rawSensor = readSensor();

  int sensorValue = sensorBuffer.getAverage();

  Millis time = millis();
  Millis timeSinceLastTick = time - lastTickTime;

  if (isTick(sensorValue)) {
    Serial.println("tick");
    Serial.print(" timeSinceLastTick ");
    Serial.println(timeSinceLastTick);

    int perMinute = millisPerMinute / timeSinceLastTick;

    if (perMinute < 500) {
      Serial.print(" perMinute ");
      Serial.println(perMinute);

      perMinuteBuffer.insert(perMinute);

      displayCadence(perMinuteBuffer.getAverage());

      lastTickTime = time;
    }
  }
}
