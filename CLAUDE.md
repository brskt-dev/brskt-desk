# CLAUDE.md

Contexto e regras para agentes neste repositório.

## Papéis

- **Usuário (dono do produto):** define o produto e valida o comportamento no hardware. Um consultor o apoia em escopo e decisões técnicas.
- **Agente:** executor técnico. **Não amplia o escopo por conta própria.** Decisões técnicas em aberto vão para o usuário, nunca são tomadas em silêncio.

## Hardware

Waveshare **ESP32-S3-Touch-LCD-7, versão com touch** (não é 7B nem 7C). Detalhes, pinagem, fontes e estado de validação: [docs/hardware.md](docs/hardware.md). Resumo do que mais pesa no código:

- ESP32-S3 rev v0.2 com 8 MB de PSRAM octal embutida e **8 MB de flash** (medido na placa; a página do produto diz 16 MB). PSRAM não equivale a memória interna: buffers DMA e ISR ficam em RAM interna.
- LCD RGB 800×480 (ST7262), touch GT911 e expansor CH422G, todos no I²C GPIO8/9 (`I2C_NUM_0`, driver legado do exemplo).
- CH422G: EXIO1 = reset do touch, EXIO2 = backlight, EXIO4 = SD_CS, EXIO5 = USB(0)/CAN(1).
- **USB nativa e CAN compartilham GPIO19/20.** Nunca presuma uso simultâneo.
- Pinagem, temporização RGB, PSRAM e backlight/reset vêm **somente** do exemplo oficial (`waveshareteam/ESP32-S3-Touch-LCD-7` @ `bc51a84`, `examples/ESP-IDF/09_lvgl_v9_demo`). Não invente valores.

## Ambiente

- Windows nativo + ESP-IDF **v5.5.6** instalado pelo EIM em `C:\Espressif`. Sem WSL, Docker ou PlatformIO.
- O terminal **não** herda o ambiente ESP-IDF automaticamente. Antes de usar `idf.py`, ative o ambiente (ver [docs/setup-windows.md](docs/setup-windows.md)):
  ```powershell
  . C:\Espressif\tools\Microsoft.v5.5.6.PowerShell_profile.ps1
  ```
  Alternativa sem ativar o shell: `eim run "idf.py build" v5.5.6`.
- A porta COM varia por máquina. Descubra com `[System.IO.Ports.SerialPort]::GetPortNames()` e nunca a versione.

## Comandos (dentro de `projects/<projeto>`)

```powershell
idf.py build                        # compila
idf.py -p COMx flash                # grava app + bootloader + partições
idf.py -p COMx monitor              # monitor serial (sair: Ctrl+])
idf.py -p COMx flash monitor
idf.py size                         # uso de memória
idf.py menuconfig                   # config interativa; persista em sdkconfig.defaults
```

## Regras de trabalho

1. Cada pasta em `projects/` é um projeto ESP-IDF independente. Sem launcher, sem plugins, sem componentes compartilhados até haver uso concreto em dois projetos.
2. Versione `sdkconfig.defaults`, `idf_component.yml` e `dependencies.lock`. Não versione `sdkconfig`, `build/`, `managed_components/` nem `.vscode/`.
3. Fixe versões de componentes (sem `^`/`*`) compatíveis com o exemplo oficial. Para atualizar, valide de novo na placa.
4. Código reaproveitado mantém o cabeçalho de licença original e é registrado em `THIRD_PARTY_NOTICES.md` do projeto, com repositório, commit e caminho.
5. Separe sempre os três estados: **código implementado**, **build aprovado** e **comportamento validado na placa**. Tela ou touch funcionando só pode ser afirmado com confirmação física do usuário.
6. Não versione senhas, tokens, caminhos pessoais nem porta COM fixa.
7. **Não** use `idf.py erase-flash` / `esptool erase_flash` como diagnóstico padrão. Só com motivo explícito e aprovação.
8. Não faça push nem merge sem pedido explícito. Trabalhe em branch.
9. Fora de escopo até nova decisão: Wi-Fi, backend, MQTT, OTA, sensores externos e funcionalidades de produto.
10. Divergências entre documentação, exemplo e placa física: registre em `docs/hardware.md` e peça confirmação. Não escolha em silêncio.
