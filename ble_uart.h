/**
 * @defgroup BLE_UART
 * @{
 *
 * @file ble_uart.h
 *
 * @brief Módulo que enmascara y adapta al proyecto la librería NUS (Nordic UART Service).
 *
 * @paragraph
 *
 * Posee buffers de recepción y transmisión que permiten enviar y recibir más de 20 bytes (limitación de NUS).
 *
 * Nordic UART Service es una librería proporcionada por Nordic. BLE NUS es un servicio patentado BLE,
 * que tiene un servicio llamado "UART Service" para imitar una comunicación serial sobre Bluetooth.
 * NUS configura un "RX" (característica con propiedades de "escritura") y un canal de transmisión "TX"
 * (característica con propiedades de "notificación"), para adaptarse a las necesidades básicas de comunicación UART.
 * Depende de SOFTDEVICE y TIMER LIBRARY.
 *
 * SOFTDEVICE es el encargado de manejar la radio del SoC. Es un archivo binario precompilado proporcionado por Nordic
 * que implementa el stack Bluetooth.
 *
 * TIMER LIBRARY es una librería proporcionada por Nordic que permite crear diferentes timers en software partiendo
 * de un mismo recurso de hardware (RTC1).
 */

#include <stdint.h>
#include <stdbool.h>

#define APP_TIMER_PRESCALER             0       /**< Value of the RTC1 PRESCALER register. */
#define MAX_LEN                         50      /**< Largo de mensaje maximo. */

/**@brief Función de inicialización del módulo.
 */
void ble_uart_init(void);


/**@brief Función para setear la flag donde indicar la llegada de un mensaje completo.
 *
 * @details Se considera que un mensaje esta completo cuando se recibe el caracter "\n".
 *
 * @param main_rx_flag    Puntero a una flag donde se indicara la llegada de un mensaje completo.
 */
void ble_uart_rx_set_flag(volatile uint8_t*);


/**@brief Función para setear la flag donde indicar el fin de transmisión de un mensaje completo.
 *
 * @param main_tx_flag    Puntero a una flag donde se indicara la llegada de un mensaje completo.
 */
void ble_uart_tx_set_flag(volatile uint8_t*);


/**@brief Función para obtener el mensaje recibido.
 *
 * @param msg_main    Puntero al arreglo donde se copiara el mensaje.
 *
 * @return El largo del mensaje.
 */
uint16_t ble_uart_get_msg(uint8_t*);

/**@brief Función para enviar un mensaje.
 *
 * @param msg_main  Puntero al arreglo donde se encuentra el mensaje.
 * @param length    El largo del mensaje.
 *
 */
void ble_uart_data_send(uint8_t * p_data, uint16_t length);


/**@brief Función iniciar el descubrimiento por Bluetooth.
 *
 */
void ble_uart_advertising_start(void);


/**@brief Función detener el descubrimiento por Bluetooth.
 */
void ble_uart_advertising_stop(void);
