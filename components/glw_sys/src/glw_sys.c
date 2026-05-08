#include "config.h"
#include "hal.h"

// implements
#include "glw_sys.h"

#define LATCH_PIN    GLW_SYS_LATCH_PIN
#define CHRG_EN_PIN  GLW_SYS_CHRG_EN_PIN
#define BUTTON_PIN   GLW_SYS_BUTTON_PIN
#define LED_EN_PIN   GLW_SYS_LED_EN_PIN
#define USB_CHRG_PIN GLW_SYS_USB_CHRG_PIN

void glw_sys_init(void) {
    set_pin_mode(LATCH_PIN, OUTPUT);
    set_level(LATCH_PIN, 1);

    set_pin_mode(LED_EN_PIN, OUTPUT);
    set_level(LED_EN_PIN, 0);  // LEDs disabled at init

    set_pin_mode(CHRG_EN_PIN, OUTPUT);
    set_level(CHRG_EN_PIN, 1);  // keep charger always enabled for now
}

void glw_sys_shutdown(void) {
    set_level(LATCH_PIN, 0);
}

void glw_sys_led_en(bool enabled) {
    set_level(LED_EN_PIN, enabled ? 1 : 0);
}
