#include <stdatomic.h>
#include "freertos/idf_additions.h"
#include "portmacro.h"

#include "log.h"
#include "config.h"
#include "glw_script.h"
#include "glw_script_priv.h"


static TaskHandle_t interpreter_task;
static atomic_flag is_buffer_taken = ATOMIC_FLAG_INIT;
static char script_buffer[INPUT_BUFFER_SIZE];

glw_script_time_t get_time_ms(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

bool wait_for_input(char *buffer, glw_script_time_t timeout_ms) {
    if(!ulTaskNotifyTake(pdTRUE, timeout_ms / portTICK_PERIOD_MS))
        return false;

    strcpy(buffer, script_buffer);
    atomic_flag_clear(&is_buffer_taken);

    return true;
}

bool glw_script_run_string(const char * script_str) {
    if(atomic_flag_test_and_set(&is_buffer_taken)) return false;
    strcpy(script_buffer, script_str);
    xTaskNotifyGive(interpreter_task);
    return true;
}

static void script_task(void * param) {
    run_interpreter();
}

void glw_script_start(void) {
    xTaskCreate(script_task, "script_runner", CONFIG_SCRIPT_TASK_STACK_SIZE, NULL, 2, &interpreter_task);
}

void glw_script_stop() {
    vTaskDelete(interpreter_task);
}
