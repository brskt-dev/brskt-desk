/*
 * 00-board-check: validação da Waveshare ESP32-S3-Touch-LCD-7.
 *
 * Etapa 1: logs de boot (chip, flash, PSRAM, heap) e heartbeat serial.
 * Etapa 2b: tela simples com identificação, uptime e um botão com contador.
 *
 * A inicialização do LCD, do touch e do backlight é a do exemplo oficial
 * (waveshare_rgb_lcd_port.c, sem alterações) e a sequência do LVGL segue
 * examples/ESP-IDF/09_lvgl_v9_demo/main/main.c (CC0-1.0, Espressif).
 * Ver THIRD_PARTY_NOTICES.md.
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "esp_app_desc.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_lv_adapter.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "waveshare_rgb_lcd_port.h"

#define HEARTBEAT_PERIOD_MS 5000

static const char *TAG = "board_check";

static lv_obj_t *s_uptime_label;
static lv_obj_t *s_count_label;
static uint32_t s_count;

/* ---------------------------------------------------------------- etapa 1 */

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

/* ---------------------------------------------------------------- etapa 2 */

/* Callbacks do LVGL rodam na tarefa do adapter, já com o lock adquirido. */

static void uptime_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    const int64_t s = esp_timer_get_time() / 1000000;
    lv_label_set_text_fmt(s_uptime_label, "Uptime: %02d:%02d:%02d",
                          (int)(s / 3600), (int)((s / 60) % 60), (int)(s % 60));
}

static void counter_button_cb(lv_event_t *e)
{
    (void)e;
    s_count++;
    lv_label_set_text_fmt(s_count_label, "Contador: %" PRIu32, s_count);
    ESP_LOGI(TAG, "botao pressionado: contador=%" PRIu32, s_count);
}

static void create_ui(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(scr, 24, 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_label_set_text(title, "brskt-desk / 00-board-check");

    lv_obj_t *info = lv_label_create(scr);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_16, 0);
    lv_label_set_text_fmt(info, "Waveshare ESP32-S3-Touch-LCD-7  |  ESP-IDF %s  |  LVGL %d.%d.%d  |  app %s",
                          esp_get_idf_version(),
                          LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH,
                          esp_app_get_description()->version);

    s_uptime_label = lv_label_create(scr);
    lv_obj_set_style_text_font(s_uptime_label, &lv_font_montserrat_24, 0);

    lv_obj_t *button = lv_button_create(scr);
    lv_obj_set_size(button, 240, 96);
    lv_obj_add_event_cb(button, counter_button_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *button_label = lv_label_create(button);
    lv_obj_set_style_text_font(button_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(button_label, "+1");
    lv_obj_center(button_label);

    s_count_label = lv_label_create(scr);
    lv_obj_set_style_text_font(s_count_label, &lv_font_montserrat_24, 0);
    lv_label_set_text(s_count_label, "Contador: 0");

    uptime_timer_cb(NULL);
    lv_timer_create(uptime_timer_cb, 1000, NULL);
}

static void display_start(void)
{
    /* Mesma sequência do exemplo oficial 09_lvgl_v9_demo. */
    const esp_lv_adapter_rotation_t rotation = ESP_LV_ADAPTER_ROTATE_0;
    const esp_lv_adapter_tear_avoid_mode_t tear_mode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_DEFAULT_RGB;

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_touch_handle_t touch_handle = NULL;

    ESP_ERROR_CHECK(waveshare_esp32_s3_rgb_lcd_init(tear_mode, rotation, &panel_handle, &touch_handle));
    ESP_ERROR_CHECK(waveshare_rgb_lcd_backlight_on());

    esp_lv_adapter_config_t adapter_config = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    adapter_config.task_stack_size = 12 * 1024;
    adapter_config.stack_in_psram = true;
    ESP_ERROR_CHECK(esp_lv_adapter_init(&adapter_config));

    esp_lv_adapter_display_config_t disp_config = ESP_LV_ADAPTER_DISPLAY_RGB_DEFAULT_CONFIG(
        panel_handle, NULL, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES, rotation);
    disp_config.profile.use_psram = true;

    lv_display_t *disp = esp_lv_adapter_register_display(&disp_config);
    assert(disp != NULL);

    if (touch_handle != NULL) {
        esp_lv_adapter_touch_config_t touch_config = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, touch_handle);
        lv_indev_t *touch = esp_lv_adapter_register_touch(&touch_config);
        assert(touch != NULL);
    }

    ESP_ERROR_CHECK(esp_lv_adapter_start());

    if (esp_lv_adapter_lock(-1) == ESP_OK) {
        create_ui();
        esp_lv_adapter_unlock();
    }
    ESP_LOGI(TAG, "UI pronta (touch %s)", touch_handle != NULL ? "registrado" : "ausente");
}

void app_main(void)
{
    log_board_info();
    display_start();
    log_heap("apos UI:");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS));
        ESP_LOGI(TAG, "alive: uptime %" PRId64 " s, contador %" PRIu32,
                 esp_timer_get_time() / 1000000, s_count);
    }
}
