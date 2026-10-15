#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"

#include "log.h"
#include "gatt/gatt.h"
#include "gap/gap.h"

#include "glw_comms_ble.h"

void ble_store_config_init(void);

static void on_reset(int reason) {
    LOGW("Resetting BLE stack; reason=%d\n", reason);
}

static void on_sync(void) {
    LOGI("BLE stack synchronized");
    gap_start();
}

static void host_task(void *param) {
    LOGI("BLE host task started");
    nimble_port_run();

    LOGE("BLE host stopped");
    nimble_port_freertos_deinit();
}

void glw_comms_ble_init(glw_comms_ble_init_t init) {
    esp_err_t ret = nimble_port_init();
    ESP_ERROR_CHECK(ret);

    ble_hs_cfg.reset_cb = on_reset;
    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_store_config_init();

    gatt_init_t gatt_init_struct = {
        .command_cb = init.command_cb
    };
    gatt_init(gatt_init_struct);

    nimble_port_freertos_init(host_task);
}

void glw_comms_ble_stop(void) {
    gap_disconnect();

    int rc = nimble_port_stop();
    if (rc == 0) {
        nimble_port_deinit();
    } else {
        LOGE("Nimble port stop failed, rc = %d", rc);
    }
}

void glw_comms_ble_send_cmd_err(const char *prefix, const char *output) {
    uint16_t conn_handle = gap_get_active_connection();
    if (conn_handle != BLE_HS_CONN_HANDLE_NONE)
        gatt_send_cmd_err(conn_handle, prefix, output);
}
