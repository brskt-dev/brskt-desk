# Código de terceiros

## Waveshare ESP32-S3-Touch-LCD-7: porte do LCD RGB e do touch

| Campo | Valor |
|---|---|
| Repositório | https://github.com/waveshareteam/ESP32-S3-Touch-LCD-7 |
| Commit | `bc51a84b30c0cea018fee6e0d7427ec10102060e` |
| Origem | `examples/ESP-IDF/09_lvgl_v9_demo/components/waveshare_rgb_lcd_port.{c,h}` |
| Destino | `main/waveshare_rgb_lcd_port.{c,h}` |
| Modificações | `.h`: **nenhuma** (blob git `4827d3da`, igual ao upstream). `.c`: em `waveshare_esp32_s3_touch_reset()`, dois bytes do CH422G mudaram, `0x2C`→`0x0C` e `0x2E`→`0x0E`, para manter o EXIO5 baixo (USB nativa ligada). Foi incluído um comentário marcando a alteração; o resto é igual ao upstream (blob original `850f172b`). Decisão do usuário em 2026-11-09, registrada em `docs/hardware.md`. |
| Licença do arquivo | `SPDX-License-Identifier: CC0-1.0` (cabeçalho do `.c`, © Espressif Systems) |
| Licença do repositório | Apache-2.0, cópia em [LICENSES/waveshare-ESP32-S3-Touch-LCD-7-Apache-2.0.txt](LICENSES/waveshare-ESP32-S3-Touch-LCD-7-Apache-2.0.txt) |

`main/main.c` segue a sequência de inicialização de `examples/ESP-IDF/09_lvgl_v9_demo/main/main.c` (mesmo commit, `SPDX-License-Identifier: CC0-1.0`, © Espressif Systems).

## Componentes do ESP Component Registry

Baixados na compilação para `managed_components/` (não versionado). As versões exatas estão em `dependencies.lock`, e cada componente traz a própria licença dentro do pacote.
