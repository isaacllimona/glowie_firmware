#include "glw_comms_ble.h"
#include "glw_comms_commands.h"

#include "glw_comms.h"

static glw_comms_ble_error_t on_command(const char *cmd) {
    glw_comms_commands_error_t ret = glw_comms_commands_run(cmd);
    switch(ret) {
        case CMD_ERR_BAD_ARGUMENTS:
            return BLE_ERR_BAD_COMMAND;
        case CMD_ERR_UNKNOWN_COMMAND:
            return BLE_ERR_BAD_COMMAND;
        case CMD_ERR_OK:
            return 0;
    }
}

void glw_comms_init(glw_comms_init_t init) {
    glw_comms_ble_init_t ble_init = {
        .command_cb = on_command
    };
    glw_comms_ble_init(ble_init);

    glw_comms_commands_init_t cmd_init = {
        .callbacks = init.callbacks
    };
    glw_comms_commands_init(cmd_init);
}

void glw_comms_stop(void) {
    glw_comms_ble_stop();
}

void glw_comms_send_err(const char *prefix, const char *msg) {
    glw_comms_ble_send_cmd_err(prefix, msg);
}
