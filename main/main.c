#include <stdbool.h>

#include "glw_os/task.h"
#include "nvs_flash.h"

#include "glw_commands.h"
#include "glw_comms.h"
#include "glw_led.h"
#include "glw_script.h"
#include "glw_sys.h"
#include "glw_os/time.h"

#include "log.h"

static const char *startup_script = "v=0 while v<20 do v=v+2 led.setColorHSV(95,170,v) sys.sleep(30) end while v>0 do v=v-1 led.setColorHSV(95,170,v) sys.sleep(30) end";

static void command_error_cb(char *msg) { LOGE("COMMAND ERROR: %s", msg); }

static void run_script(const char *script) {
    if(!glw_script_run_string(script)){
        LOGE("Could not run script. Another script is already queued.");
    }
}

static void force_shutdown_after_delay(void *args) {
    os_delay(3000);
    LOGE("Clean up is taking too long. Shutting down...");
    glw_sys_shutdown();
    os_delay(5000);
}

static void shutdown(void) {
    // glw_comms_stop() may block forever. Force shutdown after some time
    os_task_create(force_shutdown_after_delay, "force_shutdown_timer", 1000, NULL, 24);

    LOGI("Shutdown requested. Cleaning up...");

    // Turn off the LEDs. Just required if the hardware does not disconnect the LEDs on shutdown
    glw_script_stop();
    glw_led_set_color(0,0,0);

    glw_comms_stop();

    LOGI("BLE stopped. Shutting down...");

    glw_sys_shutdown();
}

void app_main(void) {
    glw_sys_init();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    glw_led_init();

    const lua_led_lib_cfg_t led_lib_cfg = {
        .set_color_rgb = glw_led_set_color,
        .set_color_hsv = glw_led_set_color_hsv,
    };
    lua_led_lib_cfg(&led_lib_cfg);

    glw_script_start();

    run_script(startup_script);

    const glw_command_setup_t command_setup = {.cmd_callbacks = {.led_en = glw_sys_led_en,
                                                                 .brightness_set = glw_led_set_brightness,
                                                                 .shutdown = shutdown,
                                                                 .script_run = run_script},
                                               .error_cb = command_error_cb};
    glw_command_setup(&command_setup);

    const glw_comms_init_t comms_init_struct = {.callbacks = {.command_cb = glw_command_run}};
    glw_comms_init(comms_init_struct);

    while (1) {
        int vbatt = glw_sys_batt_voltage_read_milivolts();
        LOGI("battery voltage: %d", vbatt);
        os_delay(5000);
    }
}
