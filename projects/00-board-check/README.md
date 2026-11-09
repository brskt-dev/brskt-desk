# 00-board-check

Firmware de validação da Waveshare ESP32-S3-Touch-LCD-7 (placa V1.1). Não tem funcionalidade de produto.

## Etapas

| Etapa | O que faz | Código | Build | Validado na placa |
|---|---|---|---|---|
| 1. Firmware mínimo | Logs de boot (chip, flash, PSRAM, heap) e heartbeat a cada 5 s | implementado | aprovado (0 warnings) | **validado** em 2026-11-06, via USB nativa |
| 2a. Exemplo oficial | `09_lvgl_v9_demo` da Waveshare, sem alterações | externo ([como rodar](../../docs/setup-windows.md#8-rodar-o-exemplo-oficial-da-waveshare)) | aprovado (3 warnings de Kconfig do próprio exemplo) | **validado** em 2026-11-09: tela, backlight e touch confirmados pelo usuário |
| 2b. Tela simples | Identificação, uptime e botão `+1` com contador, usando a inicialização do exemplo oficial (com o ajuste do EXIO5) | implementado | aprovado (0 warnings, app de 645 KB, 38% livre) | **validado** em 2026-11-09: tela confirmada pelo usuário, toques confirmados pelo usuário e pelo log |

O estado atual do código é a etapa 2b, que inclui os logs da etapa 1. A etapa 1 isolada está no commit `038a5f0`.

## Como compilar e gravar

Pré-requisito: ambiente ESP-IDF v5.5.6 ativado ([docs/setup-windows.md](../../docs/setup-windows.md)).

```powershell
cd projects\00-board-check
idf.py build
idf.py -p COMx flash monitor
```

As dependências (LVGL 9.5.0, `esp_lvgl_adapter` 0.5.3, `esp_lcd_touch_gt911` 1.2.1 e transitivas) estão fixadas em `main/idf_component.yml` e `dependencies.lock`. São as mesmas versões e hashes que o exemplo oficial resolveu.

## O que observar

### Tela

- Título `brskt-desk / 00-board-check`.
- Uma linha com placa, versão do ESP-IDF, versão da LVGL e versão do app (`git describe`).
- `Uptime: HH:MM:SS`, avançando a cada segundo.
- Botão `+1`. Cada toque soma 1 em `Contador: N`.

### Serial (USB nativa ou porta UART)

Saída real observada em 2026-11-09:

```
I (697) board_check: app board_check 038a5f0-dirty, ESP-IDF v5.5.6
I (703) board_check: chip esp32s3 rev v0.2, 2 nucleo(s), WiFi BLE
I (709) board_check: flash: configurada 8 MB, fisica (JEDEC) 8 MB
I (715) board_check: PSRAM: inicializada=sim, tamanho 8192 KB
I (720) board_check: boot: heap livre: interna 239 KB, DMA 232 KB, PSRAM 7675 KB
I (727) board_check: motivo do reset: 11
I (731) lcd_port: Install RGB LCD panel driver
I (783) lcd_port: Initialize RGB LCD panel
I (1185) lcd_port: Initialize I2C panel IO
I (1185) lcd_port: Initialize touch controller GT911
I (1191) GT911: TouchPad_ID:0x39,0x31,0x31
I (1194) GT911: TouchPad_Config_Version:88
I (1255) board_check: UI pronta (touch registrado)
I (1261) board_check: apos UI: heap livre: interna 68 KB, DMA 60 KB, PSRAM 5412 KB
I (2379) board_check: botao pressionado: contador=1
I (6262) board_check: alive: uptime 5 s, contador 19
```

- `motivo do reset: 11` = `ESP_RST_USB`: reset disparado pela USB Serial/JTAG ao abrir o monitor.
- O aviso `i2c: This driver is an old driver` é esperado: o porte oficial usa o driver I²C legado.
- Depois da UI sobram cerca de 68 KB de RAM interna: os bounce buffers do RGB e a LVGL ficam em memória interna. Leve isso em conta antes de acrescentar algo.

## Código de terceiros

`main/waveshare_rgb_lcd_port.{c,h}` vêm do exemplo oficial (CC0-1.0), e o `.c` tem um ajuste no EXIO5. Ver [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
