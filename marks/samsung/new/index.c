#include <./../main.c>

homekit_characteristic_t target_temperature = HOMEKIT_CHARACTERISTIC_(
    TARGET_TEMPERATURE,
    22,
    .min_step = (float[]){1},
    .min_value = (float[]){18},
    .max_value = (float[]){30},
    .callback = HOMEKIT_CHARACTERISTIC_CALLBACK(on_update));

void setTemperature(int temperature)
{
  printf("Send header");
  // Header
  ir_mark(2980);
  ir_space(9240);

  uint8_t samsungTemplate[] = {0x01, 0xE2, 0xFE, 0x21, 0x23, 0x11, 0xF0};

  switch (temperature)
  {
  case 18:
    break;
  case 19:
    samsungTemplate[1] = 0xD2;
    samsungTemplate[4] = 0x33;
    break;
  case 20:
    samsungTemplate[4] = 0x43;
    break;
  case 21:
    samsungTemplate[1] = 0xD2;
    samsungTemplate[4] = 0x53;
    break;
  case 22:
    samsungTemplate[1] = 0xD2;
    samsungTemplate[4] = 0x63;
    break;
  case 23:
    samsungTemplate[1] = 0xC2;
    samsungTemplate[4] = 0x73;
    break;
  case 24:
    samsungTemplate[4] = 0x83;
    break;
  case 25:
    samsungTemplate[1] = 0xD2;
    samsungTemplate[4] = 0x93;
    break;
  case 26:
    samsungTemplate[1] = 0xD2;
    samsungTemplate[4] = 0xA3;
    break;
  case 27:
    samsungTemplate[1] = 0xC2;
    samsungTemplate[4] = 0xB3;
    break;
  case 28:
    samsungTemplate[1] = 0xD2;
    samsungTemplate[4] = 0xC3;
    break;
  case 29:
    samsungTemplate[1] = 0xC2;
    samsungTemplate[4] = 0xD3;
    break;
  case 30:
    samsungTemplate[1] = 0xC2;
    samsungTemplate[4] = 0xE3;
    break;
  }

  ir_mark(440);
  ir_space(1548);
  ir_mark(2980);
  ir_space(9240);

  printf("Send template");
  for (size_t i = 0; i < sizeof(samsungTemplate); i++)
  {
    sendIRbyte(samsungTemplate[i], 440, 520, 1548);
  }

  // End mark
  ir_mark(440);
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