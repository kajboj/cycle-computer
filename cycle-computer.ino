#include <TM1637Display.h>

#define CLK 4
#define DIO 3
TM1637Display display(CLK, DIO);

const int flexPin = A0;

class Buffer {
  public:
    Buffer(unsigned int);
    void insert(int value);
    int average();

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

int Buffer::average() {
  return sum / size;
}

int previousSensorValue;
int previousSign;
Buffer buffer(32);

void setup() {
  Serial.begin(9600);
  display.setBrightness(0x08);
  previousSensorValue = 0;
  previousSign = 0;
  pinMode(flexPin, INPUT);
}

int readSensor() {
  return analogRead(flexPin);
}

int signChange() {
}

void loop() {
  int rawSensor = readSensor();
  /*Serial.print("rawSensor: ");*/
  /*Serial.println(rawSensor);*/
  /*display.showNumberDec(rawSensor, false, 4, 0);*/

  buffer.insert(rawSensor);
  int sensorValue = buffer.average();

  display.showNumberDec(sensorValue, false, 4, 0);

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
