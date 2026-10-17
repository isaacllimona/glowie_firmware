#include <string.h>
#include <assert.h>

#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs_mbuf.h"
#include "os/os_mbuf.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "log.h"
#include "config.h"
#include "uuid.h"

#include "gatt.h"

#define COMMS_COMMAND_BUFFER_SIZE   CONFIG_COMMS_COMMAND_BUFFER_SIZE

static_assert(strlen(CONFIG_DEVICE_NAME) <= MYNEWT_VAL_BLE_SVC_GAP_DEVICE_NAME_MAX_LENGTH,
              "Device name too long");

static command_cb_t command_cb;

// ===============================================
//             SERVICE DEFINITIONS
// ===============================================

// ---- Control service ----

static uint16_t char_command_handle;
static int on_command(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static uint16_t char_cmd_out_handle;
static int on_cmd_out(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def control_srvc_def = {
    .type = BLE_GATT_SVC_TYPE_PRIMARY,
    .uuid = UUID_SERVICE_CONTROL,
    .characteristics = (struct ble_gatt_chr_def[]) {
        {
            .uuid = UUID_CHAR_COMMAND,
            .access_cb = on_command,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = &char_command_handle,
        },
        {
            .uuid = UUID_CHAR_CMD_OUT,
            .access_cb = on_cmd_out,
            .flags = BLE_GATT_CHR_F_INDICATE,
            .val_handle = &char_cmd_out_handle,
        },
        {0}
    }
};

// ---- List of services ----

static const struct ble_gatt_svc_def service_definitions[] = {
    control_srvc_def,
    {0}
};

// ===============================================
//        CHARACTERISTIC ACCESS CALLBACKS
// ===============================================

// ==== Control service

static int on_command(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);
    assert(attr_handle == char_command_handle);

    char input_buffer[COMMS_COMMAND_BUFFER_SIZE];
    uint16_t written_len;

    if(ble_hs_mbuf_to_flat(ctxt->om, input_buffer, sizeof(input_buffer) - 1, &written_len) != 0) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
    input_buffer[written_len] = 0;

    LOGD("Received %u bytes of command input: %s", written_len, input_buffer);

    glw_comms_ble_error_t ret = command_cb(input_buffer);
    return ret;
}

static int on_cmd_out(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    LOGW("Unexpected access to cmd_out characteristic. Type: %d", ctxt->op);
    return 0;
}

// ===============================================
//                  INTERFACE
// ===============================================

int gatt_init(gatt_init_t init) {
    command_cb = init.command_cb;

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

void gatt_send_cmd_err(uint16_t conn_handle, const char *prefix, const char *output) {
    struct os_mbuf *om = ble_hs_mbuf_from_flat(prefix, strlen(prefix));
    os_mbuf_append(om, output, strlen(output));

    int ret = ble_gatts_notify_custom(conn_handle, char_cmd_out_handle, om);
    if (ret != 0) LOGE("Failure sending cmd output, return code: %i", ret);
}
