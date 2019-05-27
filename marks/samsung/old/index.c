#include <./../main.c>

#define CARRIER_AIRCON2_HDR_MARK 4510
#define CARRIER_AIRCON2_HDR_SPACE 4470
#define CARRIER_AIRCON2_BIT_MARK 600
#define CARRIER_AIRCON2_ONE_SPACE 1560
#define CARRIER_AIRCON2_ZERO_SPACE 500
#define CARRIER_AIRCON2_MODE_AUTO 0x10
#define CARRIER_AIRCON2_FAN_AUTO     0x05
#define CARRIER_AIRCON2_MODE_OFF     0x00 // Power OFF
#define CARRIER_AIRCON2_MODE_ON      0x20 // Power ON
#define CARRIER_AIRCON2_MODE_COOL    0x00

homekit_characteristic_t target_temperature = HOMEKIT_CHARACTERISTIC_(
    TARGET_TEMPERATURE,
    22,
    .min_step = (float[]){1},
    .min_value = (float[]){17},
    .max_value = (float[]){30},
    .callback = HOMEKIT_CHARACTERISTIC_CALLBACK(on_update));

void setTemperature(int temperature)
{
  uint8_t sendBuffer[] = {0x4D, 0xB2, 0xFD, 0x00, 0x00, 0x00};

  switch (temperature)
    {
    case 17:
      break;
    case 18:
      sendBuffer[4] = 0x08;
      break;
    case 19:
      sendBuffer[4] = 0x0C;
      break;
    case 20:
      sendBuffer[4] = 0x04;
      break;
    case 21:
      sendBuffer[4] = 0x06;
      break;
    case 22:
      sendBuffer[4] = 0x0E;
      break;
    case 23:
      sendBuffer[4] = 0x0A;
      break;
    case 24:
      sendBuffer[4] = 0x02;
      break;
    case 25:
      sendBuffer[4] = 0x03;
      break;
    case 26:
      sendBuffer[4] = 0x0B;
      break;
    case 27:
      sendBuffer[4] = 0x09;
      break;
    case 28:
      sendBuffer[4] = 0x01;
      break;
    case 29:
      sendBuffer[4] = 0x05;
      break;
    case 30:
      sendBuffer[4] = 0x0D;
      break;
    }

  // Checksums
  sendBuffer[3] = ~sendBuffer[2];
  sendBuffer[5] = ~sendBuffer[4];

  // Header
  ir_mark(CARRIER_AIRCON2_HDR_MARK);
  ir_space(CARRIER_AIRCON2_HDR_SPACE);

  // Payload
  for (size_t i = 0; i < sizeof(sendBuffer); i++)
  {
    sendIRbyte(sendBuffer[i], CARRIER_AIRCON2_BIT_MARK, CARRIER_AIRCON2_ZERO_SPACE, CARRIER_AIRCON2_ONE_SPACE);
  }

  // New header
  ir_mark(CARRIER_AIRCON2_BIT_MARK);
  ir_space(CARRIER_AIRCON2_HDR_SPACE);
  ir_mark(CARRIER_AIRCON2_HDR_MARK);
  ir_space(CARRIER_AIRCON2_HDR_SPACE);

  // Payload again
  for (size_t i = 0; i < sizeof(sendBuffer); i++)
  {
    sendIRbyte(sendBuffer[i], CARRIER_AIRCON2_BIT_MARK, CARRIER_AIRCON2_ZERO_SPACE, CARRIER_AIRCON2_ONE_SPACE);
  }

  // End mark
  ir_mark(CARRIER_AIRCON2_BIT_MARK);
  ir_space(0);
}

void off()
{
    printf("OFF");
    uint8_t sendBuffer[] = {0x4D, 0xB2, 0xDE, 0x21, 0x07, 0xF8};

    // Header
    ir_mark(CARRIER_AIRCON2_HDR_MARK);
    ir_space(CARRIER_AIRCON2_HDR_SPACE);

    // Payload
    for (size_t i = 0; i < sizeof(sendBuffer); i++)
    {
      sendIRbyte(sendBuffer[i], CARRIER_AIRCON2_BIT_MARK, CARRIER_AIRCON2_ZERO_SPACE, CARRIER_AIRCON2_ONE_SPACE);
    }

    // New header
    ir_mark(CARRIER_AIRCON2_BIT_MARK);
    ir_space(CARRIER_AIRCON2_HDR_SPACE);
    ir_mark(CARRIER_AIRCON2_HDR_MARK);
    ir_space(CARRIER_AIRCON2_HDR_SPACE);

    // Payload again
    for (size_t i = 0; i < sizeof(sendBuffer); i++)
    {
      sendIRbyte(sendBuffer[i], CARRIER_AIRCON2_BIT_MARK, CARRIER_AIRCON2_ZERO_SPACE, CARRIER_AIRCON2_ONE_SPACE);
    }

    // End mark
    ir_mark(CARRIER_AIRCON2_BIT_MARK);
    ir_space(0);
}