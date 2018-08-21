#include <./../main.c>

#define HDR_MARK 3100
#define HDR_SPACE 3000
#define BIT_MARK 426
#define ONE_SPACE 1067
#define ZERO_SPACE 275
#define PAUSE_SPACE 0

#define MODE_ON    0x04
#define MODE_COOL  0x03
#define FAN_AUTO   0x00
#define VS_AUTO    0x00

homekit_characteristic_t target_temperature = HOMEKIT_CHARACTERISTIC_(
    TARGET_TEMPERATURE,
    22,
    .min_step = (float[]){0.5},
    .min_value = (float[]){16},
    .max_value = (float[]){31},
    .callback = HOMEKIT_CHARACTERISTIC_CALLBACK(on_update));

void setTemperature(int temperature)
{
  ir_mark(HDR_MARK);
  ir_space(HDR_SPACE);

  uint8_t header[] = { 0x23, 0xCB, 0x26, 0x02, 0x00, 0x40, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65
  };

  uint8_t template[] = { 0x23, 0xCB, 0x26, 0x01, 0x00, 0xA4, 0x03,
                         0x00, 0x05, 0x00, 0x00, 0x00, 0x80, 0x41
  };

  for (size_t i = 0; i < sizeof(header); i++)
  {
    sendIRbyte(header[i], BIT_MARK, ZERO_SPACE, ONE_SPACE);
  }

  ir_mark(BIT_MARK);
  ir_space(HDR_SPACE);
  ir_mark(HDR_MARK);
  ir_space(HDR_SPACE);

  for (size_t i = 0; i < sizeof(template); i++)
  {
    sendIRbyte(template[i], BIT_MARK, ZERO_SPACE, ONE_SPACE);
  }

  ir_mark(BIT_MARK);
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