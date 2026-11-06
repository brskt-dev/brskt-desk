/*
 * 00-board-check — etapa 1: firmware mínimo.
 *
 * Confirma gravação, boot e logs seriais, e reporta o que o chip detecta
 * (revisão, flash física, PSRAM inicializada e heap por capacidade).
 * Não toca em LCD, touch, expansor de I/O nem barramento I2C.
 */

#include <inttypes.h>
#include <stdbool.h>

#include "esp_app_desc.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define HEARTBEAT_PERIOD_MS 5000

static const char *TAG = "board_check";

static void log_heap(const char *prefix)
{
    ESP_LOGI(TAG, "%s heap livre: interna %u KB, DMA %u KB, PSRAM %u KB",
             prefix,
             (unsigned)(heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024),
             (unsigned)(heap_caps_get_free_size(MALLOC_CAP_DMA) / 1024),
             (unsigned)(heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024));
}

static void log_board_info(void)
{
    const esp_app_desc_t *app = esp_app_get_description();
    ESP_LOGI(TAG, "app %s %s, ESP-IDF %s", app->project_name, app->version, esp_get_idf_version());

    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(TAG, "chip %s rev v%d.%d, %d nucleo(s),%s%s",
             CONFIG_IDF_TARGET,
             chip.revision / 100, chip.revision % 100,
             chip.cores,
             (chip.features & CHIP_FEATURE_WIFI_BGN) ? " WiFi" : "",
             (chip.features & CHIP_FEATURE_BLE) ? " BLE" : "");

    uint32_t flash_cfg = 0;
    uint32_t flash_phys = 0;
    esp_flash_get_size(NULL, &flash_cfg);
    esp_flash_get_physical_size(NULL, &flash_phys);
    ESP_LOGI(TAG, "flash: configurada %" PRIu32 " MB, fisica (JEDEC) %" PRIu32 " MB",
             flash_cfg / (1024 * 1024), flash_phys / (1024 * 1024));

    ESP_LOGI(TAG, "PSRAM: inicializada=%s, tamanho %u KB",
             esp_psram_is_initialized() ? "sim" : "nao",
             (unsigned)(esp_psram_get_size() / 1024));

    log_heap("boot:");
    ESP_LOGI(TAG, "motivo do reset: %d", (int)esp_reset_reason());
}

void app_main(void)
{
    log_board_info();

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS));
        ESP_LOGI(TAG, "alive: uptime %" PRId64 " s", esp_timer_get_time() / 1000000);
    }
}
