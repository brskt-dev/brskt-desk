# 00-board-check

Firmware de validação da Waveshare ESP32-S3-Touch-LCD-7. Não tem funcionalidade de produto.

## Etapas

| Etapa | O que faz | Código | Build | Validado na placa |
|---|---|---|---|---|
| 1. Firmware mínimo | Logs de boot (chip, flash, PSRAM, heap) e heartbeat a cada 5 s | implementado | aprovado (0 warnings) | **validado** em 2026-11-06 via USB nativa (COM3) |
| 2a. Exemplo oficial | `09_lvgl_v9_demo` da Waveshare, sem alterações: LCD, backlight e touch | (externo) | pendente | pendente |
| 2b. Tela simples | Identificação, uptime e botão com contador, usando a inicialização do exemplo oficial | pendente | pendente | pendente |

## Como compilar e gravar

Pré-requisito: ambiente ESP-IDF v5.5.6 ativado ([docs/setup-windows.md](../../docs/setup-windows.md)).

```powershell
cd projects\00-board-check
idf.py build
idf.py -p COMx flash monitor
```

## O que observar

### Etapa 1 (logs)

Saída real observada em 2026-11-06:

```
I (179) esp_psram: Found 8MB PSRAM device
I (491) esp_psram: SPI SRAM memory test OK
I (612) board_check: app board_check be0cc8a-dirty, ESP-IDF v5.5.6
I (618) board_check: chip esp32s3 rev v0.2, 2 nucleo(s), WiFi BLE
I (624) board_check: flash: configurada 8 MB, fisica (JEDEC) 8 MB
I (630) board_check: PSRAM: inicializada=sim, tamanho 8192 KB
I (635) board_check: boot: heap livre: interna 323 KB, DMA 316 KB, PSRAM 8019 KB
I (642) board_check: motivo do reset: 11
I (5646) board_check: alive: uptime 5 s
```

`motivo do reset: 11` = `ESP_RST_USB`: reset disparado pela USB Serial/JTAG ao abrir o monitor.

Os logs saem nas duas portas: UART0 (porta **UART**, CH343) e USB Serial/JTAG (porta **USB**).
