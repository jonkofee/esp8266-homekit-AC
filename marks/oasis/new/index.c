#include <./../main.c>

#define HDR_MARK 3080
#define HDR_SPACE 3000
#define BIT_MARK 420
#define ONE_SPACE 1060
#define ZERO_SPACE 320
#define PAUSE_SPACE 0

#define MODE_ON    0x24
#define MODE_OFF   0x00
#define MODE_COOL  0x03
#define FAN2       0x03
#define VS_AUTO    0x00
#define MODE_COOL  0x03

homekit_characteristic_t target_temperature = HOMEKIT_CHARACTERISTIC_(
    TARGET_TEMPERATURE,
    17,
    .min_step = (float[]){1},
    .min_value = (float[]){16},
    .max_value = (float[]){31},
    .callback = HOMEKIT_CHARACTERISTIC_CALLBACK(on_update));

void setTemperature(int temperature)
{
    handle(true, temperature);
}

void off()
{
    handle(false, 22);
}

void handle(bool isOn, int temperature)
{
    uint8_t header[] = { 0x23, 0xCB, 0x26, 0x02, 0x00, 0x40, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65
    };
    uint8_t template[] = { 0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x00,
                           0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00
    };
    uint8_t checksum = 0x00;

    // Set the operatingmode on the template message
    template[5] |= MODE_OFF;
    template[6] |= MODE_COOL;

    // Set the temperature on the template message
    template[7] |= 31 - 22;

    // Set the fan speed and vertical air direction on the template message
    template[8] = 0x03;
    // Calculate the checksum
    for (unsigned int i=0; i < (sizeof(template)-1); i++) {
      checksum += template[i];
    }

    template[13] = checksum;

    ir_mark(HDR_MARK);
    ir_space(HDR_SPACE);

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