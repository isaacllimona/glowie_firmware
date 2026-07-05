#pragma once

#include "esp_log.h"

#define SET_LOG_TAG(tag) static const char log_tag[] = tag

#define LOGD(msg, ...) ESP_LOGD(log_tag, msg, ##__VA_ARGS__)
#define LOGI(msg, ...) ESP_LOGI(log_tag, msg, ##__VA_ARGS__)
#define LOGW(msg, ...) ESP_LOGW(log_tag, msg, ##__VA_ARGS__)
#define LOGE(msg, ...) ESP_LOGE(log_tag, msg, ##__VA_ARGS__)

#define STRINGIFY_(x) #x
#define STRINGIFY(x)  STRINGIFY_(x)
#define LOC_STR       __FILE__ ":" STRINGIFY(__LINE__)

#define LOGW_LOC(msg, ...) LOGW(LOC_STR ": " msg, ##__VA_ARGS__)
#define LOGE_LOC(msg, ...) LOGE(LOC_STR ": " msg, ##__VA_ARGS__)
