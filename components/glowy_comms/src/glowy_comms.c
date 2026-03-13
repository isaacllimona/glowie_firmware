#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"

#include "log.h"
#include "gatt/gatt.h"
#include "gap/gap.h"

#include "glowy_comms.h"

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

void glowy_comms_init(glowy_comms_init_t init) {
    esp_err_t ret = nimble_port_init();
    ESP_ERROR_CHECK(ret);

    ble_hs_cfg.reset_cb = on_reset;
    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_store_config_init();

    gatt_init_t gatt_init_struct = {.callbacks = init.callbacks};
    gatt_init(gatt_init_struct);

    nimble_port_freertos_init(host_task);
}
