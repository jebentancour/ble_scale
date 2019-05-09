#include <stdint.h>
#include <stdbool.h>

/**@brief Función de inicialización del módulo.
 */
void gpio_init(void);

/**@brief Función que setea la flag para avisar un click.
 */
void gpio_button_set_flag(volatile uint8_t* main_button_flag);

/**@brief Función que setea la flag para avisar un long click.
 */
void gpio_long_button_set_flag(volatile uint8_t* main_long_button_flag);

/**@brief Función que setea la flag para avisar doble click.
 */
void gpio_double_button_set_flag(volatile uint8_t* main_double_button_flag);

/**@brief Función para polling del botón.
 *
 * @param now Tiempo de ejecución en ms.
 */
void gpio_process(uint32_t now);
