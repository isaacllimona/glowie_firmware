#include <stdio.h>
#include <string.h>

#include "esp_netif_types.h"
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/message_buffer.h"
#include "portmacro.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi_default.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"

#define LOG_TAG "main"
#define DELAY_MS(t) vTaskDelay(t/portTICK_PERIOD_MS)

#define SSID CONFIG_WIFI_AP_SSID
#define PASSWORD CONFIG_WIFI_AP_PASSWORD

#define PORT CONFIG_TCP_PORT

#define MIN_PACKET_SIZE 400
#define MAX_PACKET_SIZE 480

static void transmit(const int sock, MessageBufferHandle_t message_buffer) {
    xMessageBufferReset(message_buffer);

    while(1) {
        // Accumulate data into a single packet until it reaches the minimum size
        char tx_buffer[MAX_PACKET_SIZE];
        size_t tx_size = 0;
        while (tx_size < MIN_PACKET_SIZE) {
            size_t frame_size = xMessageBufferReceive(message_buffer, (void *)(tx_buffer + tx_size), MAX_PACKET_SIZE - tx_size, portMAX_DELAY);
            if(frame_size == 0) break;  // Send packet early if next message does not fit in the current packet
            tx_size += frame_size;
        }
    
        ESP_LOGI(LOG_TAG, "Transmitting %u bytes...", tx_size);

        int to_write = tx_size;
        while(to_write > 0) {
            int written = send(sock, tx_buffer + (tx_size - to_write), to_write, 0);
            if(written < 0) {
                ESP_LOGE(LOG_TAG, "Error while sending");
                return;
            }
            to_write -= written;
        }
    }
}

static void tcp_server_task(void *params) {
    MessageBufferHandle_t message_buffer = (MessageBufferHandle_t) params;

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // wait for ip

    struct sockaddr_in dest_addr = {
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };

    char addr_str[128];

    int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_socket < 0) {
        ESP_LOGE(LOG_TAG, "Unable to create socket: errno %d", errno);
        return;
    }

    ESP_LOGI(LOG_TAG, "Socket created");

    int err = bind(listen_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if(err != 0){
        ESP_LOGE(LOG_TAG, "Error occurred during binding: errno %d", errno);

        close(listen_socket);
        vTaskDelete(NULL);
    }

    ESP_LOGI(LOG_TAG, "Socket bound. Listening...");

    err = listen(listen_socket, 1);
    if (err != 0) {
        ESP_LOGE(LOG_TAG, "Error occurred during listen: errno %d", errno);

        close(listen_socket);
        vTaskDelete(NULL);
    }

    while(1) {
        ESP_LOGI(LOG_TAG, "Socket listening");

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        int sock = accept(listen_socket, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(LOG_TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Log connection
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
        ESP_LOGI(LOG_TAG, "Socket accepted ip address: %s", addr_str);

        transmit(sock, message_buffer);

        shutdown(sock, 0);
        close(sock);
    }
}

TaskHandle_t tcp_server_task_handle;

static void on_wifi_event_start(){
    ESP_LOGI(LOG_TAG, "WIFI started. Connecting...");
    esp_wifi_connect();
}

static void on_wifi_event_disconnected(){
    ESP_LOGI(LOG_TAG, "WIFI disconnected");
    esp_wifi_connect();
}

static void on_wifi_event_connected(){
    ESP_LOGI(LOG_TAG, "WIFI connected");
}

static void on_wifi_event_got_ip(){
    xTaskNotify(tcp_server_task_handle, 1, eNoAction);
}

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch(event_id) {
            case WIFI_EVENT_STA_START:
                on_wifi_event_start();
                break;
            case WIFI_EVENT_STA_CONNECTED:
                on_wifi_event_connected();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                on_wifi_event_disconnected();
                break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        on_wifi_event_got_ip();
    }
}

void tcp_server_start(MessageBufferHandle_t message_buffer) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_init_config.nvs_enable = 0;
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID,
        event_handler, NULL
    );
    esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP,
        event_handler, NULL
    );

    wifi_config_t wifi_config = {.sta = {
        .ssid = SSID,
        .password = PASSWORD,
    }};

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    xTaskCreate(tcp_server_task, "tcp_server_app", 20000, (void*) message_buffer, 1, &tcp_server_task_handle);
}
