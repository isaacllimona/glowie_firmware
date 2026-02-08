#include <stdatomic.h>
#include "freertos/idf_additions.h"
#include "portmacro.h"

#include "log.h"
#include "glowy_script.h"
#include "glowy_script_priv.h"


static TaskHandle_t interpreter_task;
static atomic_flag is_buffer_taken = ATOMIC_FLAG_INIT;
static char script_buffer[INPUT_BUFFER_SIZE];

glowy_script_time_t get_time_ms(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

bool wait_for_input(char *buffer, glowy_script_time_t timeout_ms) {
    if(!ulTaskNotifyTake(pdTRUE, timeout_ms / portTICK_PERIOD_MS))
        return false;

    strcpy(buffer, script_buffer);
    atomic_flag_clear(&is_buffer_taken);

    return true;
}

bool glowy_script_run_string(char * script_str) {
    if(atomic_flag_test_and_set(&is_buffer_taken)) return false;
    strcpy(script_buffer, script_str);
    xTaskNotifyGive(interpreter_task);
    return true;
}

static void script_task(void * param) {
    run_interpreter();
}

void glowy_script_start(void) {
    xTaskCreate(script_task, "script_runner", 5000, NULL, 2, &interpreter_task);
}
