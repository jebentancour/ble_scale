#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define NRF_LOG_MODULE_NAME "MAIN"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "clock.h"
#include "button.h"
#include "ble_uart.h"
#include "batt.h"
#include "rgb_led.h"
#include "HX711.h"

volatile uint8_t clock_tick_flag;

#define SEND_INTERVAL 1000

uint32_t now;
uint32_t last_send;

volatile uint8_t button_flag;
volatile uint8_t long_button_flag;
volatile uint8_t double_button_flag;

volatile uint8_t ble_uart_rx_flag;
volatile uint8_t ble_uart_tx_flag;
uint8_t ble_uart_rx_msg[MAX_LEN];
uint8_t ble_uart_tx_msg[MAX_LEN];
uint16_t ble_msg_len;

volatile uint8_t batt_flag;

int main(void){
    NRF_LOG_INIT(clock_get_timestamp);
    NRF_LOG_INFO("-------------------- ble scale --------------------\n");
    NRF_LOG_FLUSH();

    sd_power_mode_set(NRF_POWER_MODE_LOWPWR);

    ble_uart_rx_flag = 0;
    ble_uart_rx_set_flag(&ble_uart_rx_flag);
    ble_uart_tx_flag = 1;
    ble_uart_tx_set_flag(&ble_uart_tx_flag);
    ble_uart_init();

    clock_tick_flag = 0;
    clock_tick_set_flag(&clock_tick_flag);
    clock_init();

    rgb_led_init();
    rgb_led_full(BLACK);

    button_flag = 0;
    gpio_button_set_flag(&button_flag);
    long_button_flag = 0;
    gpio_long_button_set_flag(&long_button_flag);
    double_button_flag = 0;
    gpio_double_button_set_flag(&double_button_flag);
    gpio_init();

    batt_flag = 0;
    batt_set_flag(&batt_flag);
    batt_init();

    batt_sample();
    while(!batt_flag){
        // wait...
    }
    batt_flag = 0;

    uint32_t voltage;
    voltage = batt_get();
    NRF_LOG_INFO("VCC = %d.%d V\n", voltage / 1000, voltage % 1000);
    NRF_LOG_FLUSH();

    HX711_init();

    while (true){
        now = clock_get_timestamp();

        rgb_led_update(now); // update LEDs animation
        gpio_process(now);   // GPIO polling

        if (button_flag){
          button_flag = 0;
          ble_uart_advertising_start();
          NRF_LOG_INFO("button_flag\n");
          NRF_LOG_FLUSH();
        }

        if (long_button_flag){
          long_button_flag = 0;
          HX711_tare(10);
          NRF_LOG_INFO("long_button_flag\n");
          NRF_LOG_FLUSH();
        }

        if (double_button_flag){
          double_button_flag = 0;
          NRF_LOG_INFO("double_button_flag\n");
          NRF_LOG_FLUSH();
        }

        if (ble_uart_rx_flag){
          ble_uart_rx_flag = 0;
          ble_msg_len = ble_uart_get_msg(ble_uart_rx_msg);
          NRF_LOG_INFO("ble_uart_rx_flag\n");
          NRF_LOG_HEXDUMP_INFO(ble_uart_rx_msg, ble_msg_len);
          NRF_LOG_FLUSH();
        }

        if (now - last_send > SEND_INTERVAL){
          if (ble_uart_tx_flag == 1){
            ble_msg_len = sprintf((char*)ble_uart_tx_msg, "%ld\n", HX711_get_units(2));
            ble_uart_data_send(ble_uart_tx_msg, ble_msg_len);
            last_send = now;
          }
        }
    }
}
