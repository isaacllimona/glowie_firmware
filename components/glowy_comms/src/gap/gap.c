#include "esp_log.h"

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/util/util.h"

#include "sdkconfig.h"

#include "../uuid.h"
#include "gap.h"


#define TAG "comms"

#ifdef CONFIG_USE_RANDOM_ADDRESS
    #define USE_RANDOM_ADDRESS 1
#else
    #define USE_RANDOM_ADDRESS 0
#endif


inline static void format_addr(char *addr_str, uint8_t addr[]) {
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1],
            addr[2], addr[3], addr[4], addr[5]);
}

static void get_own_addr(uint8_t *addr_type, uint8_t addr_val[]) {
    int rc = ble_hs_util_ensure_addr(USE_RANDOM_ADDRESS);
    assert(rc == 0);

    ble_hs_id_infer_auto(CONFIG_USE_RANDOM_ADDRESS, addr_type);
    ble_hs_id_copy_addr(*addr_type, addr_val, NULL);
}

static int on_gap_event(struct ble_gap_event *event, void *arg);

uint8_t addr_type;

static void advertisement_setup(void) {
    uint8_t addr_val[6];
    get_own_addr(&addr_type, addr_val);

    char addr_str[18] = {0};
    format_addr(addr_str, addr_val);
    ESP_LOGI(TAG, "Using device address: %s", addr_str);

    const char *name = CONFIG_DEVICE_NAME;
    const struct ble_hs_adv_fields fields = {
        .flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP,

        .uuids128 = (ble_uuid128_t[]){*BLE_UUID128(UUID_SERVICE_CONTROL)},
        .num_uuids128 = 1,
        .uuids128_is_complete = 0,

        .name = (uint8_t *)name,
        .name_len = strlen(name),
        .name_is_complete = 1,

        .tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO,
        .tx_pwr_lvl_is_present = 1,
    };

    int rc = ble_gap_adv_set_fields(&fields);
    assert(rc == 0);
    // An error here may be caused by too many advertisement fields
    // (max payload of adv. packet is 31 bytes)
}

static void advertisement_start(void) {
    ESP_LOGI(TAG, "Starting advertisement");
    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
    };
    int rc = ble_gap_adv_start(addr_type, NULL, BLE_HS_FOREVER, &adv_params, on_gap_event, NULL);
    assert(rc == 0);
}

static int log_connection_details(uint16_t conn_handle) {
    struct ble_gap_conn_desc desc;
    int rc = ble_gap_conn_find(conn_handle, &desc);
    if(rc != 0) {
        ESP_LOGE(TAG, "failed to find connection by handle, error code: %d", rc);
        return rc;
    }

    char addr_str[18] = {0};

    ESP_LOGI(TAG, "connection handle: %d", desc.conn_handle);

    format_addr(addr_str, desc.our_id_addr.val);
    ESP_LOGI(TAG, "device id address: type=%d, value=%s",
             desc.our_id_addr.type, addr_str);

    format_addr(addr_str, desc.peer_id_addr.val);
    ESP_LOGI(TAG, "peer id address: type=%d, value=%s", desc.peer_id_addr.type,
             addr_str);

    ESP_LOGI(TAG,
             "conn_itvl=%d, conn_latency=%d, supervision_timeout=%d, "
             "encrypted=%d, authenticated=%d, bonded=%d\n",
             desc.conn_itvl, desc.conn_latency, desc.supervision_timeout,
             desc.sec_state.encrypted, desc.sec_state.authenticated,
             desc.sec_state.bonded);

    return 0;
}

static int on_gap_event(struct ble_gap_event *event, void *arg) {
    switch(event->type) {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "connection %s", event->connect.status == 0 ? "established" : "failed");
            if(event->connect.status == 0) {
                int rc = log_connection_details(event->connect.conn_handle);
                return rc;
            }
            else {
                advertisement_start();
                return 0;
            }

        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "disconnected from peer; reason=%d", event->disconnect.reason);
            advertisement_start();
            return 0;

        case BLE_GAP_EVENT_CONN_UPDATE:
            ESP_LOGI(TAG, "connection updated; status=%d", event->conn_update.status);
            int rc = log_connection_details(event->connect.conn_handle);
            return rc;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI(TAG, "advertise complete; reason=%d", event->adv_complete.reason);
            advertisement_start();
            return 0;

        default:
            ESP_LOGI(TAG, "unhandled GAP event; type=%d", event->type);
            return 0;

    }
}

void gap_start(void){
    advertisement_setup();
    advertisement_start();
}
