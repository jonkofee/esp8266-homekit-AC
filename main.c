#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "wifi.h"

homekit_characteristic_t target_temperature;
void update_ac_state();
void offAC();
void setACTemp(int temperature);

const int ir_gpio = 4;

float target_temperature_value = 22.0;
int mode = 0;

void sendIRbyte(uint8_t sendByte, int bitMarkLength, int zeroSpaceLength, int oneSpaceLength)
{
  for (int i=0; i<8 ; i++)
  {
    if (sendByte & 0x01)
    {
      ir_mark(bitMarkLength);
      ir_space(oneSpaceLength);
    }
    else
    {
      ir_mark(bitMarkLength);
      ir_space(zeroSpaceLength);
    }

    sendByte >>= 1;
  }
}

void setACTemp(int temperature) {
    printf("Send header");
    // Header
    ir_mark(2980);
    ir_space(9240);

    uint8_t samsungTemplate[] = { 0x01, 0xE2, 0xFE, 0x21, 0x23, 0x11, 0xF0 };

    switch (temperature) {
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
    for (size_t i = 0; i < sizeof(samsungTemplate); i++) {
      sendIRbyte(samsungTemplate[i], 440, 520, 1548);
    }

  // End mark
  ir_mark(440);
  ir_space(0);

}

void offAC() {
    printf("OFF");
  static const uint8_t samsungOffCode[] = { 0x01, 0x72, 0x0F, 0x00, 0x70, 0x41, 0x01, 0x01, 0xD2, 0xFE, 0x21, 0xD3, 0x15, 0xC0 };

  // Header
  ir_mark(2980);
  ir_space(9240);

  for (size_t i=0; i<sizeof(samsungOffCode); i++) {
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

void on_set(homekit_value_t val) {
    target_temperature_value = val.float_value;

    update_ac_state();
}

homekit_value_t on_get() {
    return HOMEKIT_FLOAT(target_temperature_value);
}

void on_set_mode(homekit_value_t val) {
    mode = val.int_value;
    
    update_ac_state();
}

void update_ac_state() {
    printf("Current mode: %d\n", mode);

    if (mode == 0) {
        offAC();
    } else {
        setACTemp((int) target_temperature_value);
    }
}

homekit_characteristic_t current_temperature = HOMEKIT_CHARACTERISTIC_(
    CURRENT_TEMPERATURE, 19
);
homekit_characteristic_t target_temperature  = HOMEKIT_CHARACTERISTIC_(
    TARGET_TEMPERATURE,
    22,
    .min_step = (float []) {1},
    .min_value = (float []) {18},
    .max_value = (float []) {30},
    .setter = on_set,
    .getter = on_get
);
homekit_characteristic_t units = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0);
homekit_characteristic_t current_state = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0);
homekit_characteristic_t target_state = HOMEKIT_CHARACTERISTIC_(
    TARGET_HEATING_COOLING_STATE,
    0,
    .setter = on_set_mode
);
homekit_characteristic_t cooling_threshold = HOMEKIT_CHARACTERISTIC_(
    COOLING_THRESHOLD_TEMPERATURE, 25
);
homekit_characteristic_t heating_threshold = HOMEKIT_CHARACTERISTIC_(
    HEATING_THRESHOLD_TEMPERATURE, 15
);
homekit_characteristic_t current_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);

static void wifi_init() {
    struct sdk_station_config wifi_config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
    };

    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&wifi_config);
    sdk_wifi_station_connect();
}

static void init_ir() {
    gpio_enable(ir_gpio, GPIO_OUTPUT);
    gpio_write(ir_gpio, 0);

    ir_set_pin(ir_gpio);

    ir_set_frequency(38);
}

void thermostat_identify(homekit_value_t _value) {
    printf("Thermostat identify\n");
}


homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_thermostat, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Кондиционер"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "OASIS"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "001"),
            HOMEKIT_CHARACTERISTIC(MODEL, "undefined"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, thermostat_identify),
            NULL
        }),
        HOMEKIT_SERVICE(THERMOSTAT, .primary=true, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "Кондиционер"),
            &current_temperature,
            &target_temperature,
            &current_state,
            &target_state,
            &cooling_threshold,
            &heating_threshold,
            &units,
            &current_humidity,
            NULL
        }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-111"
};

void user_init(void) {
    uart_set_baud(0, 115200);

    init_ir();

    wifi_init();
    homekit_server_init(&config);
}
