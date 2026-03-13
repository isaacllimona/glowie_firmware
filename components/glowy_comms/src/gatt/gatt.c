#include <string.h>
#include <assert.h>

#include "log.h"

#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "sdkconfig.h"
#include "glowy_comms.h"
#include "uuid.h"
#include "gatt.h"


#define TAG "comms_gatt"

#define COMMS_SCRIPT_IN_BUFFER_SIZE CONFIG_COMMS_SCRIPT_IN_BUFFER_SIZE

static_assert(strlen(CONFIG_DEVICE_NAME) <= MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME_MAX_LENGTH,
              "Device name too long");

static glowy_comms_callbacks_t callbacks;

// ===============================================
//             SERVICE DEFINITIONS
// ===============================================

// ---- Control service ----

static uint16_t control_enable_handle;
static int on_enable_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static uint16_t control_brightness_handle;
static int on_brightness_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static uint16_t control_rgb_handle;
static int on_rgb_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def control_srvc_def = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = UUID_SERVICE_CONTROL,
    .characteristics = (struct ble_gatt_chr_def[]) {
        {
            .uuid = UUID_CHAR_ENABLE,
            .access_cb = on_enable_write,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = &control_enable_handle,
            .cpfd = (struct ble_gatt_cpfd[]) {
                {
                    .format = BLE_GATT_CHR_FORMAT_BOOLEAN,
                    .unit = BLE_GATT_CHR_UNIT_UNITLESS,
                    .name_space = BLE_GATT_CHR_NAMESPACE_BT_SIG,
                },
                {0}
            },
        },
        {
            .uuid = UUID_CHAR_BRIGHTNESS,
            .access_cb = on_brightness_write,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = &control_brightness_handle,
            .cpfd = (struct ble_gatt_cpfd[]) {
                {
                    .format = BLE_GATT_CHR_FORMAT_UINT8,
                    .unit = BLE_GATT_CHR_UNIT_UNITLESS,
                    .name_space = BLE_GATT_CHR_NAMESPACE_BT_SIG,
                },
                {0}
            }
        },
        {
            .uuid = UUID_CHAR_RGB,
            .access_cb = on_rgb_write,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = &control_rgb_handle,
            .cpfd = (struct ble_gatt_cpfd[]) {
                {
                    .format = BLE_GATT_CHR_FORMAT_UINT24,
                    .unit = BLE_GATT_CHR_UNIT_UNITLESS,
                    .name_space = BLE_GATT_CHR_NAMESPACE_BT_SIG,
                },
                {0}
            },
        },
        {0}
    }
};

// ---- Script service ----

static uint16_t script_in_handle;
static int on_script_in(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def script_srvc_def = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = UUID_SERVICE_SCRIPT,
    .characteristics = (struct ble_gatt_chr_def[]) {
        {
            .uuid = UUID_CHAR_SCRIPT_IN,
            .access_cb = on_script_in,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = &script_in_handle,
        },
        {0}
    }
};

// ---- List of services ----

static const struct ble_gatt_svc_def service_definitions[] = {
    control_srvc_def,
    script_srvc_def,
    {0}
};

// ===============================================
//        CHARACTERISTIC ACCESS CALLBACKS
// ===============================================

// ==== Control service

static int on_enable_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == control_enable_handle);
    if(ctxt->om->om_len != 1) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    uint8_t val = ctxt->om->om_data[0];
    if (val > 1) return 0xFF;  // value out of range (accept only 0 or 1)

    LOGD("Received write to onoff characteristic. Value: %d", val);
    callbacks.set_enable_cb(val);

    return 0;
}

static int on_brightness_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == control_brightness_handle);
    if(ctxt->om->om_len != 1) return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;

    uint8_t val = ctxt->om->om_data[0];
    LOGD("Received write to brightness characteristic. Value: %d", val);
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

    LOGD("Received write to rgb characteristic. Value: %d, %d, %d", r, g, b);
    callbacks.set_color_rgb_cb(r, g, b);

    return 0;
}

// ==== Script service

static int on_script_in(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == script_in_handle);

    char input_buffer[COMMS_SCRIPT_IN_BUFFER_SIZE];
    uint16_t written_len;

    if(ble_hs_mbuf_to_flat(ctxt->om, input_buffer, sizeof(input_buffer) - 1, &written_len) != 0) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
    input_buffer[written_len] = 0;

    LOGD("Received %u bytes of script input: %s", written_len, input_buffer);

    if(callbacks.script_in_cb(input_buffer)) return 0;
    else return 0xFE;
}

// ===============================================
//                  INTERFACE
// ===============================================

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
