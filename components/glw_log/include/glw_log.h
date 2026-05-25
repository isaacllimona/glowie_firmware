#pragma once

#include "esp_log.h"

#define SET_LOG_TAG(tag) static const char log_tag[] = tag

#define LOGD(msg, ...) ESP_LOGD(log_tag, msg, ##__VA_ARGS__)
#define LOGI(msg, ...) ESP_LOGI(log_tag, msg, ##__VA_ARGS__)
#define LOGW(msg, ...) ESP_LOGW(log_tag, msg, ##__VA_ARGS__)
#define LOGE(msg, ...) ESP_LOGE(log_tag, msg, ##__VA_ARGS__)
