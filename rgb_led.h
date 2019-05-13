#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BLACK,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN
} led_color_t;

/**@brief Función de inicialización del módulo.
 */
void rgb_led_init(void);

void rgb_led_set_color(led_color_t color);

void rgb_led_full(led_color_t color);

void rgb_led_toggle(led_color_t color, uint16_t period);

void rgb_led_breathe(led_color_t color, uint16_t period, uint16_t grain);

void rgb_led_fade(uint16_t time, uint16_t grain);

void rgb_led_bright(uint8_t br);

/**@brief Función que actualiza el estado de las animaciones RGB.
 *
 * @param now Tiempo de ejecución en ms.
 */
void rgb_led_update(uint32_t now);

void rgb_led_show(void);

/**@brief Función que apaga todos los LEDs.
 */
void rgb_led_disable(void);
