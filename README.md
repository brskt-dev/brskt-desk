# brskt-desk

Mini-projetos de software embarcado para a **Waveshare ESP32-S3-Touch-LCD-7**, na versão com touch: ESP32-S3, LCD RGB de 7" 800×480 e touch capacitivo GT911.

Cada pasta em `projects/` é um projeto **ESP-IDF independente**, compilado e gravado separadamente. Não há launcher, sistema de plugins nem componentes compartilhados entre projetos.

## Projetos

| Pasta | Finalidade | Estado |
|---|---|---|
| [`projects/00-board-check`](projects/00-board-check/) | Valida a placa: gravação, logs seriais, PSRAM, LCD, backlight e touch | validado na placa (V1.1) em 2026-11-09. Ver [README do projeto](projects/00-board-check/README.md) |

## Base técnica

| Item | Versão |
|---|---|
| ESP-IDF | v5.5.6 (instalado via EIM) |
| Interface | LVGL 9 + `espressif/esp_lvgl_adapter`, seguindo o exemplo oficial da Waveshare |
| Exemplo de referência | `waveshareteam/ESP32-S3-Touch-LCD-7` @ `bc51a84`, `examples/ESP-IDF/09_lvgl_v9_demo` |

As versões exatas dos componentes ficam no `dependencies.lock` de cada projeto.

## Documentação

- [docs/hardware.md](docs/hardware.md): características, pinagem, restrições, revisão e estado de validação.
- [docs/setup-windows.md](docs/setup-windows.md): instalação no Windows nativo, build, flash, monitor e diagnóstico.
- [CLAUDE.md](CLAUDE.md): contexto e regras de trabalho para agentes.

## Uso rápido

Com o ambiente ESP-IDF ativado no PowerShell (veja [docs/setup-windows.md](docs/setup-windows.md)):

```powershell
cd projects\00-board-check
idf.py build
idf.py -p COMx flash monitor
```

Troque `COMx` pela porta da sua placa. Nenhuma porta é fixada no repositório.
