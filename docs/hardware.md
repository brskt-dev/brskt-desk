# Hardware: Waveshare ESP32-S3-Touch-LCD-7

Placa-alvo de todos os projetos deste repositório. É a **versão com touch** (SKU 27078). Não confundir com a ESP32-S3-LCD-7 sem touch (SKU 30406) nem com os modelos **7B** ou **7C**, que têm outra pinagem e outros controladores.

> Legenda de status: **documentado** = consta em fonte oficial · **visto no código** = aparece no exemplo oficial · **validado na placa** = observado no hardware físico.

## Revisão

| Item | Valor | Status |
|---|---|---|
| Revisão no esquemático oficial | `ESP32-S3-Touch-LCD-7 V1.2` | documentado |
| Revisão física desta placa | — | **PENDENTE**: conferir a serigrafia no verso da placa |
| Chip | ESP32-S3 (QFN56) **rev v0.2**, PSRAM embutida de 8 MB (AP, 3,3 V) | validado na placa (`esptool flash_id` e log de boot) |
| Flash | **8 MB**, GigaDevice (JEDEC `c8` / `4017`), modo QIO, 3,3 V | validado na placa (`esptool flash_id`, log `spi_flash: detected chip: gd`) |

A Waveshare não publica histórico de revisões nem diferenças entre versões. Se a serigrafia não indicar V1.2, compare a placa com o esquemático antes de confiar na pinagem abaixo.

### Divergência: flash de 16 MB × 8 MB

A página do produto (e a especificação inicial deste repositório) diz "ESP32-S3N16R8, 16 MB de flash". **Esta placa tem 8 MB.** O `esptool` identifica um ESP32-S3 com PSRAM **embutida no encapsulamento** (ou seja, um ESP32-S3R8) e uma flash GigaDevice externa de 64 Mbit. Isso bate com o `sdkconfig.defaults` do exemplo oficial (`CONFIG_ESPTOOLPY_FLASHSIZE_8MB`) e com o README do exemplo ("ESP32-S3R8"). Os projetos deste repositório usam **8 MB**. Pode ser outra revisão ou outro lote. Confirmar pela serigrafia e, se possível, pela marcação do CI de flash.

## Características (fonte: página oficial, salvo indicação)

- MCU ESP32-S3: Xtensa LX7 dual-core até 240 MHz, 512 KB de SRAM, 384 KB de ROM, **8 MB de PSRAM octal**. A página cita módulo ESP32-S3N16R8 com 16 MB de flash; **nesta placa a flash é de 8 MB** (ver divergência acima).
- Wi-Fi 2,4 GHz 802.11 b/g/n e Bluetooth 5 (LE), com antena na placa.
- LCD IPS de 7", 800 × 480, 65K cores, interface RGB paralela de 16 bits (RGB565).
  - Controlador **ST7262**, segundo o FAQ. O README do exemplo cita ST7701, mas é um texto genérico herdado da Espressif.
- Touch capacitivo **GT911** via I²C, até 5 pontos.
- Expansor de I/O **CH422G** via I²C. Controla o reset do touch, o backlight, o CS do cartão SD e o chaveamento USB/CAN.
- CAN (TJA1051), RS485, conector I²C, conector de sensor/ADC, UART e slot microSD (SPI).
- Duas portas USB-C:
  - **USB**: USB nativa do ESP32-S3 (USB Serial/JTAG). Nesta máquina aparece como VID `303A`, PID `1001`.
  - **UART**: ponte CH343 ligada à UART0 (GPIO43/44). Uma chave DIP alterna a UART0 entre essa porta e o conector UART2.
- Alimentação de 5 V pela USB-C (450 mA). Bateria Li-ion 3,7 V opcional no conector PH2.0.
- Botões BOOT e RESET. A gravação é automática pelas duas portas.

## Fontes

| Fonte | Referência |
|---|---|
| Página do produto (docs) | https://docs.waveshare.com/ESP32-S3-Touch-LCD-7 |
| Guia ESP-IDF da Waveshare | https://docs.waveshare.com/ESP32-S3-Touch-LCD-7/ESP-IDF |
| Recursos e datasheets | https://docs.waveshare.com/ESP32-S3-Touch-LCD-7/Resources-And-Documents |
| FAQ | https://docs.waveshare.com/ESP32-S3-Touch-LCD-7/FAQ |
| Exemplos oficiais (Apache-2.0) | https://github.com/waveshareteam/ESP32-S3-Touch-LCD-7 |
| **Commit de referência** | `bc51a84b30c0cea018fee6e0d7427ec10102060e` (2026-07-25, "Fix demo docs and project naming") |
| Exemplo de referência | `examples/ESP-IDF/09_lvgl_v9_demo` (LVGL 9 + `esp_lvgl_adapter`) |
| Esquemático | `hardware/schematics/ESP32-S3-Touch-LCD-7-Sch.pdf` (no mesmo commit) |
| Datasheet CH422G | https://files.waveshare.com/wiki/common/CH422DS1_EN.pdf |
| Datasheet GT911 | https://files.waveshare.com/wiki/common/GT911_EN_Datasheet.pdf |
| Datasheet ST7262 | https://files.waveshare.com/wiki/common/ST7262.pdf |

## Pinagem

Fontes: tabelas da página oficial, conferidas com `waveshare_rgb_lcd_port.h` do exemplo de referência.

### LCD RGB (16 bits, RGB565)

| Sinal | GPIO | | Sinal | GPIO |
|---|---|---|---|---|
| PCLK | 7 | | DE | 5 |
| HSYNC | 46 | | VSYNC | 3 |
| DATA0 (B3) | 14 | | DATA8 (G5) | 48 |
| DATA1 (B4) | 38 | | DATA9 (G6) | 47 |
| DATA2 (B5) | 18 | | DATA10 (G7) | 21 |
| DATA3 (B6) | 17 | | DATA11 (R3) | 1 |
| DATA4 (B7) | 10 | | DATA12 (R4) | 2 |
| DATA5 (G2) | 39 | | DATA13 (R5) | 42 |
| DATA6 (G3) | 0 | | DATA14 (R6) | 41 |
| DATA7 (G4) | 45 | | DATA15 (R7) | 40 |

- DISP e RST do LCD não usam GPIO (`-1` no exemplo). O backlight é acionado pelo CH422G.
- GPIO0, GPIO3, GPIO45 e GPIO46 são **pinos de strapping** do ESP32-S3 e estão ocupados pelo LCD. Não os reaproveite.

### Temporização RGB (exemplo oficial, sem alterações)

| Parâmetro | Valor |
|---|---|
| Pixel clock | 16 MHz |
| Resolução | 800 × 480 |
| HSYNC pulse / back porch / front porch | 4 / 8 / 8 |
| VSYNC pulse / back porch / front porch | 4 / 8 / 8 |
| Borda do PCLK | `pclk_active_neg = 1` |
| Bounce buffer | `800 × 10 × 16 / 8` = 16000 px |
| Framebuffers | em PSRAM (`fb_in_psram = 1`). A quantidade vem de `esp_lv_adapter_get_required_frame_buffer_count()` |

### Barramento I²C (compartilhado)

| Sinal | GPIO |
|---|---|
| SDA | 8 |
| SCL | 9 |

Dispositivos no barramento: CH422G, GT911 e qualquer coisa ligada ao conector I²C externo. O exemplo usa o **driver I²C legado** (`driver/i2c.h`) na porta `I2C_NUM_0`, a 400 kHz, com pull-ups internos.

### Touch GT911

| Sinal | Ligação |
|---|---|
| TP_SDA / TP_SCL | GPIO8 / GPIO9 |
| TP_IRQ | GPIO4 |
| TP_RST | EXIO1 (CH422G) |

No exemplo, o GPIO4 é saída em nível baixo durante o reset, o que fixa o endereço do GT911 em `0x5D` (`ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG`). Depois do reset o pino continua como saída. O touch funciona por **polling** (`int_gpio_num = -1`), sem interrupção.

### Expansor CH422G

| Saída | Função | Fonte |
|---|---|---|
| EXIO1 | TP_RST (reset do touch) | documentado |
| EXIO2 | DISP: habilita o backlight (boost AP3032) | documentado |
| EXIO3 | não consta na tabela oficial; o exemplo o mantém em nível alto | visto no código |
| EXIO4 | SD_CS, ativo em nível baixo | documentado |
| EXIO5 | USB_SEL: **baixo = USB**, **alto = CAN** (switch FSUSB42) | documentado |

O exemplo fala com o CH422G por dois "endereços de comando", usando escrita I²C de 1 byte:

| Endereço (7 bits) | Uso no exemplo | Valor escrito |
|---|---|---|
| `0x24` | parâmetro de sistema: habilita as saídas IO | `0x01` |
| `0x38` | estado das saídas EXIO0–EXIO7 (bit n = EXIOn) | `0x2C`, `0x2E`, `0x1E` |

Sequência do exemplo (`waveshare_rgb_lcd_port.c`):

1. `0x24 ← 0x01`, e GPIO4 configurado como saída.
2. `0x38 ← 0x2C`: TP_RST baixo, DISP alto, EXIO3 alto, **EXIO5 alto (CAN)**.
3. Espera 100 ms, GPIO4 = 0, espera 100 ms.
4. `0x38 ← 0x2E`: libera TP_RST.
5. Espera 200 ms.
6. `waveshare_rgb_lcd_backlight_on()`: `0x24 ← 0x01`, depois `0x38 ← 0x1E` (EXIO1–4 altos, **EXIO5 baixo (USB)**).

**Consequência:** durante a inicialização do LCD e do touch, a USB nativa fica desconectada por pelo menos ~400 ms, porque o GPIO19/20 é chaveado para o CAN. Logs pela porta **USB** (USB Serial/JTAG) podem ser interrompidos e a porta COM pode reenumerar. A porta **UART** (CH343) não é afetada. Esse comportamento vem do exemplo oficial e foi mantido de propósito. Mudá-lo é decisão técnica pendente.

O exemplo só liga e desliga o backlight. Não há controle de brilho (PWM).

### Outros periféricos (não usados nesta etapa)

| Função | Pinos |
|---|---|
| USB nativa D− / D+ | GPIO19 / GPIO20 (com EXIO5 baixo) |
| CAN RX / TX | GPIO19 / GPIO20 (com EXIO5 alto) |
| RS485 TXD / RXD | GPIO15 / GPIO16 (direção automática) |
| microSD (SPI) MOSI / SCK / MISO / CS | GPIO11 / GPIO12 / GPIO13 / EXIO4 |
| UART0 TX / RX | GPIO43 / GPIO44 (CH343 ou conector UART2, pela chave DIP) |

## Restrições de projeto

1. **USB nativa × CAN são exclusivos.** Os dois usam GPIO19/20 e o EXIO5 escolhe um deles. Firmware com CAN derruba a porta USB nativa. Nesse caso, grave e monitore pela porta UART ou entre em modo de download pelo botão BOOT.
2. **I²C compartilhado.** CH422G, GT911 e dispositivos externos ficam no mesmo barramento, GPIO8/9 em `I2C_NUM_0`. Todo acesso deve passar pela mesma instância do driver. O código de LCD do exemplo usa o driver **legado**. No ESP-IDF 5.x, o driver legado e o novo (`driver/i2c_master.h`) não podem coexistir no mesmo binário (o firmware aborta no boot). Também atenção à chave de nível do conector I²C (3,3 V ou 5 V).
3. **PSRAM não é memória interna.** São 8 MB octais a 80 MHz, mapeados por cache. Servem para framebuffers, buffers do LVGL e pilhas de tarefas marcadas para PSRAM. Não servem para buffers DMA internos, código e dados usados com o cache desligado (ISR em IRAM) nem para estruturas que exigem `MALLOC_CAP_INTERNAL`/`MALLOC_CAP_DMA`. Aloque com `heap_caps_malloc()` e a capacidade explícita quando isso importar.
4. **Flash de 8 MB nesta placa**, apesar dos 16 MB da página do produto. A tabela de partições padrão (app de 1 MB) basta por enquanto. Se um projeto precisar de mais espaço, a tabela deve caber em 8 MB.
5. **Wi-Fi + RGB.** A Espressif documenta *drift* da imagem quando Wi-Fi e LCD RGB disputam a PSRAM. Fora do escopo agora, mas relevante no futuro: https://docs.espressif.com/projects/esp-faq/en/latest/software-framework/peripherals/lcd.html

## Estado de validação nesta placa

| Item | Status | Evidência |
|---|---|---|
| Placa enumerada pela USB nativa | validado (host) | Windows mostra `USB JTAG/serial debug unit`, VID 303A/PID 1001 |
| Gravação de firmware | **validado** (2026-11-06) | `idf.py -p COM3 flash` pela USB nativa, "Hash of data verified" nas 3 regiões |
| Logs seriais (USB Serial/JTAG) | **validado** (2026-11-06) | `idf.py monitor` em COM3: boot completo + heartbeat `alive: uptime N s` |
| Logs seriais (UART/CH343) | pendente | porta UART ainda não conectada |
| PSRAM 8 MB inicializada | **validado** (2026-11-06) | `esp_psram: Found 8MB PSRAM device`, `SPI SRAM memory test OK`, `PSRAM: inicializada=sim, tamanho 8192 KB` |
| Flash física | **validado: 8 MB** (diverge da página) | `Detected flash size: 8MB`, log `fisica (JEDEC) 8 MB` |
| LCD exibe imagem | pendente | etapa 2, observação visual |
| Backlight acende | pendente | etapa 2, observação visual |
| Touch responde | pendente | etapa 2, toque no botão + log |
