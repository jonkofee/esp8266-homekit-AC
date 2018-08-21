#include <./../main.c>

#define CARRIER_AIRCON1_HDR_MARK   4320
#define CARRIER_AIRCON1_HDR_SPACE  4350
#define CARRIER_AIRCON1_BIT_MARK   500
#define CARRIER_AIRCON1_ONE_SPACE  1650
#define CARRIER_AIRCON1_ZERO_SPACE 550
#define CARRIER_AIRCON1_MSG_SPACE  7400
#define CARRIER_AIRCON1_MODE_AUTO  0x00
#define CARRIER_AIRCON1_FAN_AUTO   0x00

homekit_characteristic_t target_temperature = HOMEKIT_CHARACTERISTIC_(
    TARGET_TEMPERATURE,
    22,
    .min_step = (float[]){1},
    .min_value = (float[]){17},
    .max_value = (float[]){30},
    .callback = HOMEKIT_CHARACTERISTIC_CALLBACK(on_update));

void setTemperature(int temperature)
{
  printf("Send header");
  uint8_t sendBuffer[] = {0x4f, 0xb0, 0xc0, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00};

  static const uint8_t temperatures[] = {0x00, 0x08, 0x04, 0x0c, 0x02, 0x0a, 0x06, 0x0e, 0x01, 0x09, 0x05, 0x0d, 0x03, 0x0b};
  uint8_t checksum = 0;

  sendBuffer[5] = temperatures[(temperature - 17)];

  sendBuffer[6] = CARRIER_AIRCON1_MODE_AUTO | CARRIER_AIRCON1_FAN_AUTO;

  // Checksum

  for (int i = 0; i < 8; i++)
  {
    checksum += IRbitReverse(sendBuffer[i]);
  }

  switch (sendBuffer[6] & 0xF0)
  {
  case 0x00:
    checksum += 0x02;
    switch (sendBuffer[6] & 0x0F)
    {
    case 0x02: // FAN1
    case 0x03: // FAN5
    case 0x06: // FAN2
      checksum += 0x80;
      break;
    }
    break;
  case 0x40: // MODE_DRY - all settings should work
    checksum += 0x02;
    break;
  case 0xC0: // MODE_HEAT - certain temperature / fan speed combinations do not work
    switch (sendBuffer[6] & 0x0F)
    {
    case 0x05: // FAN4
    case 0x06: // FAN2
      checksum += 0xC0;
      break;
    }
    break;
  case 0x20: // MODE_FAN - all settings should work
    checksum += 0x02;
    switch (sendBuffer[6] & 0x0F)
    {
    case 0x02: // FAN1
    case 0x03: // FAN5
    case 0x06: // FAN2
      checksum += 0x80;
      break;
    }
    break;
  }

printf("Send header2");
  sendBuffer[8] = IRbitReverse(checksum);

  // Header
  ir_mark(CARRIER_AIRCON1_HDR_MARK);
  ir_space(CARRIER_AIRCON1_HDR_SPACE);

printf("Send Payload");
  // Payload
  for (size_t i = 0; i < sizeof(sendBuffer); i++)
  {
    sendIRbyte(sendBuffer[i], CARRIER_AIRCON1_BIT_MARK, CARRIER_AIRCON1_ZERO_SPACE, CARRIER_AIRCON1_ONE_SPACE);
  }

  // Pause + new header
  ir_mark(CARRIER_AIRCON1_BIT_MARK);
  ir_space(CARRIER_AIRCON1_MSG_SPACE);

  ir_mark(CARRIER_AIRCON1_HDR_MARK);
  ir_space(CARRIER_AIRCON1_HDR_SPACE);

  // Payload again
  for (size_t i = 0; i < sizeof(sendBuffer); i++)
  {
    sendIRbyte(sendBuffer[i], CARRIER_AIRCON1_BIT_MARK, CARRIER_AIRCON1_ZERO_SPACE, CARRIER_AIRCON1_ONE_SPACE);
  }

printf("Send end");
  // End mark
  ir_mark(CARRIER_AIRCON1_BIT_MARK);
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