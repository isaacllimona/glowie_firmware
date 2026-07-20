#include <stdbool.h>

#include "glw_os/task.h"
#include "nvs_flash.h"

#include "glw_commands.h"
#include "glw_comms.h"
#include "glw_led.h"
#include "glw_script.h"
#include "glw_script_libs/led.h"
#include "glw_sys.h"
#include "glw_os/time.h"

#include "log.h"
#include "sync_clock.h"

static const char *startup_script =
    "local function fade(val) led.setColor(math.floor(84*val), math.floor(227*val), math.floor(255*val)) end"
    " local v=0; while v<0.1 do fade(v) v=v+0.01 sleep(30) end"
    " while v>=0 do fade(v) v=v-0.005 sleep(30) end"
    " led.setColor(0, 0, 0)";

static void command_err_cb(char *msg) {
    LOGE("COMMAND ERROR: %s", msg);
    glw_comms_send_cmd_err("(CMD) ", msg);
}
static void script_err_cb(const char *msg) {
    LOGE("SCRIPT ERROR: %s", msg);
    glw_comms_send_cmd_err("(SCRIPT) ", msg);
}

static void run_script(const char *script) {
    if(glw_script_run_string(script) != 0){
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

static void set_led_enabled(bool enabled) {
    // Power on/off the LEDs with the LED enable GPIO
    glw_sys_led_en(enabled);

    // Refresh the LEDs to the previous state after power on
    // Also light off the LEDs even if the hardware does not support GPIO LED disabling
    os_delay(10);  // Wait for the LEDs to power on. TODO: tune delay
    glw_led_set_enabled(enabled);
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

    const glw_script_init_t script_init = {
        .get_time = sync_clock_get,
        .output_error = script_err_cb,
    };
    glw_script_init(&script_init);
    glw_script_start();

    const glw_script_libs_led_init_t script_led_lib_init = {
        .set_color_rgb = glw_led_set_color,
        .set_pixel_color_rgb = glw_led_set_pixel_color,
        .update = glw_led_update
    };
    glw_script_libs_led_add(&script_led_lib_init);

    run_script(startup_script);

    const glw_command_setup_t command_setup = {.cmd_callbacks = {.led_en = set_led_enabled,
                                                                 .brightness_set = glw_led_set_brightness,
                                                                 .shutdown = shutdown,
                                                                 .script_run = run_script,
                                                                 .time_set = sync_clock_set},
                                               .error_cb = command_err_cb};
    glw_command_setup(&command_setup);

    const glw_comms_init_t comms_init_struct = {.callbacks = {.command_cb = glw_command_run}};
    glw_comms_init(comms_init_struct);

    while (1) {
        int vbatt = glw_sys_batt_voltage_read_milivolts();
        LOGI("battery voltage: %d", vbatt);
        os_delay(5000);
    }
}
