#include <stdint.h>
#include <stdlib.h>

#include "glw_common/log.h"
#include "glw_common/os/time.h"
#include "glw_inrt/data_dispatcher.h"
#include "glw_inrt/glw_inrt.h"
#include "glw_inrt/task.h"
#include "glw_led.h"

#define MIN(a, b) (a < b ? a : b)

SET_LOG_TAG("main");

void app_main(void) {
    glw_led_init();

    glw_inrt_init();
    glw_inrt_task_start(1);

    os_timer timer;
    os_timer_init(&timer, 30);
    while (true) {
        int z_vel = glw_inrt_get_vertical_velocity();

        LOGI("vertical velocity: %d", z_vel);

        uint8_t val = MIN(abs(z_vel / 20), 255);
        uint8_t hue = z_vel > 0 ? 100 : 60;
        glw_led_set_color_hsv(hue, 220, val);

        os_timer_sleep(&timer);
    }
}
