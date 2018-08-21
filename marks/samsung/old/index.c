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
  uint8_t sendBuffer[] = {0x4D, 0xB2, 0xD8, 0x00, 0x00, 0x00};

  static const uint8_t temperatures[] = {0, 8, 12, 4, 6, 14, 10, 2, 3, 11, 9, 1, 5, 13, 7};

  sendBuffer[2] |= CARRIER_AIRCON2_MODE_ON | CARRIER_AIRCON2_FAN_AUTO;

  sendBuffer[4] |= CARRIER_AIRCON2_MODE_COOL | temperatures[(temperature - 17)];

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
  static const uint8_t samsungOffCode[] = {0x01, 0x72, 0x0F, 0x00, 0x70, 0x41, 0x01, 0x01, 0xD2, 0xFE, 0x21, 0xD3, 0x15, 0xC0};

  // Header
  ir_mark(2980);
  ir_space(9240);

  for (size_t i = 0; i < sizeof(samsungOffCode); i++)
  {
    sendIRbyte(samsungOffCode[i], 440, 520, 1548);

    if (i == 6)
    {
      ir_mark(440);
      ir_space(1548);
      ir_mark(2980);
      ir_space(9240);
    }
  }

  // End mark
  ir_mark(440);
  ir_space(0);
}