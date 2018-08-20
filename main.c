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

void update_state();
void off();
void setTemperature(int temperature);
homekit_characteristic_t target_temperature;

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

void on_update(homekit_characteristic_t *ch, homekit_value_t value, void *context) {
    update_state();
}

homekit_characteristic_t current_temperature = HOMEKIT_CHARACTERISTIC_(
    CURRENT_TEMPERATURE, 19
);
homekit_characteristic_t units = HOMEKIT_CHARACTERISTIC_(TEMPERATURE_DISPLAY_UNITS, 0);
homekit_characteristic_t current_state = HOMEKIT_CHARACTERISTIC_(CURRENT_HEATING_COOLING_STATE, 0);
homekit_characteristic_t target_state = HOMEKIT_CHARACTERISTIC_(
    TARGET_HEATING_COOLING_STATE,
    0,
    .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(on_update)
);
homekit_characteristic_t cooling_threshold = HOMEKIT_CHARACTERISTIC_(
    COOLING_THRESHOLD_TEMPERATURE, 25
);
homekit_characteristic_t heating_threshold = HOMEKIT_CHARACTERISTIC_(
    HEATING_THRESHOLD_TEMPERATURE, 15
);
homekit_characteristic_t current_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0);

void update_state() {
    uint8_t current_state_value = current_state.value.int_value;
    uint8_t target_state_value = target_state.value.int_value;
    uint8_t current_temperature_value = (int) current_temperature.value.float_value;
    uint8_t target_temperature_value = (int) target_temperature.value.float_value;

    printf("Current mode: %d\n", current_state_value);
    printf("Target mode: %d\n", target_state_value);
    printf("Current temperature: %d\n", current_temperature_value);
    printf("Target temperature: %d\n", target_temperature_value);

    if (target_state_value > 0) {
        printf("Okey, i set %d temperature\n", target_temperature_value); 
        setTemperature(target_temperature_value);
    } else {
        if (current_state_value == target_state_value) {
            printf("AC ready OFF");

            return;
        }

        printf("Okey, now off");
        off();
    }

    current_state.value = HOMEKIT_UINT8(target_state_value);
    homekit_characteristic_notify(&current_state, current_state.value);
}

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
