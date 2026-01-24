#pragma once

#include "host/ble_gatt.h"

#include "../uuid.h"

// ---- Control service

uint16_t control_enable_handle;
static int on_enable_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

uint16_t control_brightness_handle;
static int on_brightness_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

uint16_t control_rgb_handle;
static int on_rgb_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

const struct ble_gatt_svc_def control_srvc_def = {
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
                }, {0}
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
                }, {0}
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
                }, {0}
            },
        }, {0}
    }
};
