#include <string.h>
#include <assert.h>

#include "esp_log.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "sdkconfig.h"

#include "services.h"
#include "gatt.h"

#define TAG "comms_gatt"

static_assert(strlen(CONFIG_DEVICE_NAME) <= MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME_MAX_LENGTH,
              "Device name too long");

comms_callbacks_t callbacks;

const struct ble_gatt_svc_def service_definitions[] = {
    control_srvc_def,
    {0}
};

int gatt_init(gatt_init_t init) {
    callbacks = init.callbacks;

    int rc;

    rc = ble_svc_gap_device_name_set(CONFIG_DEVICE_NAME);
    assert(rc == 0);

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(service_definitions);
    assert(rc == 0);

    rc = ble_gatts_add_svcs(service_definitions);
    assert(rc == 0);

    return 0;
}

/********* CHARACTERISTIC ACCESS CALLBACKS ***********/

static int on_enable_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == control_enable_handle);
    if(ctxt->om->om_len != 1) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    uint8_t val = ctxt->om->om_data[0];
    if (val > 1) return 0xFF;  // value out of range (accept only 0 or 1)

    ESP_LOGD(TAG, "Received write to onoff characteristic. Value: %d", val);
    callbacks.set_enable_cb(val);

    return 0;
}

static int on_brightness_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == control_brightness_handle);
    if(ctxt->om->om_len != 1) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    uint8_t val = ctxt->om->om_data[0];
    ESP_LOGD(TAG, "Received write to brightness characteristic. Value: %d", val);
    callbacks.set_brightness_cb(val);

    return 0;
}

static int on_rgb_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == control_rgb_handle);
    if(ctxt->om->om_len != 3) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    int r = ctxt->om->om_data[0];
    int g = ctxt->om->om_data[1];
    int b = ctxt->om->om_data[2];

    ESP_LOGD(TAG, "Received write to rgb characteristic. Value: %d, %d, %d", r, g, b);
    callbacks.set_color_rgb_cb(r, g, b);

    return 0;
}

