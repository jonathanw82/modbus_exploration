// arduino -> rs485 tansiever (incl. RE WE) -> SDM120 modbus device
// https://www.etteam.com/productDIN/SDM120-MODBUS/SDM120-PROTOCOL.pdf
// Each parameter is held in two consecutive16 bit register.

#include <ModbusMaster.h>

#define RE_PIN 5
#define SDM120_SLAVE_ID 1

#define VOLTAGE_ADDR 0x00
#define CURRENT_ADDR 0x06
#define ACTIVE_POWER ADDR 0x0C 
#define FREQUENCY_ADDR 0X46

ModbusMaster modbus;

void setup() {
  Serial.begin(9600);
  Serial1.begin(2400);
  // put your setup code here, to run once:
  pinMode(RE_PIN, OUTPUT);
  
  // modbus.begin(slaveid, serialinterface)
  modbus.begin(SDM120_SLAVE_ID, Serial1);
  modbus.preTransmission(preTransmission);
  modbus.postTransmission(postTransmission);
}

void loop() {
  // put your main code here, to run repeatedly:
  // result = node.readHoldingRegisters(2, 6);
  delay(1000);
  Serial.println(F("Reading data..."));

  static uint32_t i;
  uint8_t j, result;
  uint16_t data[2];

  // result = modbus.readHoldingRegisters(FREQUENCY, 2);
  result = modbus.readInputRegisters(VOLTAGE_ADDR, 2);
  Serial.println(F("modbus read holding registers"));

  // do something with data if read is successful
  if (result == modbus.ku8MBSuccess) {
    Serial.println(F("SUCCESS"));
    for (j = 0; j < 2; j++) {
      data[j] = modbus.getResponseBuffer(j);
    }
    float voltage = get_float_from_u16_array(data[0], data[1]);
    char value_str[32];
    dtostrf(voltage, 2, 3, value_str);
    Serial.print(F("Voltage = "));
    Serial.println(value_str);
  } else {
    Serial.println(F("FAILED"));
  }
}

void preTransmission()
{
  digitalWrite(RE_PIN, 1);
}
void postTransmission()
{
  digitalWrite(RE_PIN, 0);
}

float get_float_from_u16_array(uint16_t val1, uint16_t val2)
{
  // 2 x uint16 converted to floats using same memory space
  union
  {
    float asFloat;
    uint16_t asInt[2];
  } temp;
  temp.asInt[0] = val2;
  temp.asInt[1] = val1;

  return temp.asFloat;
}
