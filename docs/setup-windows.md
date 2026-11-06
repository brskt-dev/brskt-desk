# Setup no Windows (nativo)

Ambiente reproduzível para compilar, gravar e monitorar os projetos deste repositório. Sem WSL, Docker ou PlatformIO.

## Versões fixadas

| Ferramenta | Versão | Motivo |
|---|---|---|
| ESP-IDF | **v5.5.6** | O exemplo oficial depende de `espressif/esp_lvgl_adapter` 0.5.x, que exige IDF ≥ 5.5. A Waveshare documenta a linha 5.5 (capturas com v5.5.2) e o código do exemplo usa o driver I²C legado. A v5.5.6 é o patch mais recente da linha 5.5 (2026-10-16). A 6.x não foi adotada porque o exemplo não foi validado nela. |
| EIM (ESP-IDF Installation Manager), CLI | 0.20.1 | Instalador oficial atual da Espressif. É o mesmo que a extensão do VS Code usa |
| ESP-IDF commit | `880a203ae603d08a94d06aa01e0d6761958e5e9e` | `git describe` = `v5.5.6` |
| Toolchain Xtensa | `xtensa-esp-elf` GCC 14.2.0 (esp-14.2.0_20260121) | definida pelo `tools.json` do IDF v5.5.6 |
| CMake / Ninja | 3.30.2 / 1.12.1 | instalados pelo EIM em `C:\Espressif\tools` |
| esptool / esp-idf-monitor | 4.13.0 / 1.10.0 | venv do IDF |
| Python | 3.13.1 do sistema, como base da venv `C:\Espressif\tools\python\v5.5.6\venv` | o EIM usa o primeiro Python 3 compatível que encontrar |
| Extensão VS Code ESP-IDF (opcional) | 2.x | lê as instalações do EIM via `C:\Espressif\tools\eim_idf.json` |

Componentes (LVGL, adapter, driver GT911): versões exatas no `dependencies.lock` de cada projeto.

## 1. Pré-requisitos

- Git para Windows (`git --version`).
- `winget` (App Installer da Microsoft Store).
- ~5 GB livres em `C:`.
- Cabo USB-C **de dados**.

## 2. Instalar o EIM (CLI)

```powershell
winget install --id Espressif.EIM-CLI --exact --version 0.20.1
```

Abra um **novo** terminal e confira:

```powershell
eim --version
```

## 3. Instalar o ESP-IDF v5.5.6 (somente alvo esp32s3)

```powershell
eim install -i v5.5.6 -t esp32s3 -p C:\Espressif -n true --do-not-track true --cleanup true
```

- `-p C:\Espressif`: caminho curto, sem espaços. Evite caminhos com espaços ou acentos.
- `-t esp32s3`: baixa só as ferramentas necessárias.
- `--do-not-track true`: desliga a telemetria do instalador.
- Leva de 10 a 30 min, conforme a rede. Ao final, `eim list` deve mostrar `v5.5.6` com status `Finished`.

Alternativa por GUI: `winget install Espressif.EIM`. Escolha v5.5.6, alvo ESP32-S3 e caminho `C:\Espressif`.

## 4. Ativar o ambiente no terminal

Ter o ESP-IDF instalado (ou a extensão do VS Code) **não** coloca `idf.py` no PATH. Em cada novo PowerShell:

```powershell
. C:\Espressif\tools\Microsoft.v5.5.6.PowerShell_profile.ps1
idf.py --version
```

Sem ativar o shell, rodando um comando isolado:

```powershell
eim run "idf.py build" v5.5.6
```

O EIM também cria o atalho **IDF_v5.5.6_Powershell** na Área de Trabalho, que abre um PowerShell já ativado. Se o PowerShell bloquear o script, confira `Get-ExecutionPolicy -List`. Nesta máquina vale `RemoteSigned` (LocalMachine), que permite o script local.

Na saída da instalação apareceu `Error: No such command 'cooking'` (do `compote`), e a instalação mesmo assim terminou com `status: finished` e exit code 0. Não afetou build nem flash.

## 5. Build, flash e monitor

```powershell
cd projects\00-board-check
idf.py build
idf.py -p COMx flash
idf.py -p COMx monitor        # sair: Ctrl+]
```

Na primeira compilação, o gerenciador de componentes baixa as dependências para `managed_components/`, conforme o `dependencies.lock`.

### Descobrir a porta COM

```powershell
[System.IO.Ports.SerialPort]::GetPortNames()
Get-PnpDevice -PresentOnly -Class Ports | Select-Object Status, FriendlyName, InstanceId
```

| Porta física | Como aparece | VID:PID |
|---|---|---|
| USB-C **USB** (USB nativa) | "Dispositivo Serial USB (COMn)" / "USB JTAG/serial debug unit" | `303A:1001` |
| USB-C **UART** (CH343) | "USB-Enhanced-SERIAL CH343 (COMn)", esperado; ainda não observado nesta máquina | `1A86:55D3` (esperado) |

As duas portas gravam e mostram logs. Use a porta **UART** quando o firmware mexer no EXIO5 (USB/CAN), como faz a inicialização do LCD: veja [hardware.md](hardware.md#expansor-ch422g).

## 6. Diagnóstico

| Sintoma | O que fazer |
|---|---|
| `idf.py` não encontrado | Ative o ambiente (seção 4) no terminal atual. |
| Porta COM não aparece | Troque o cabo (pode ser só de carga) e confira no Gerenciador de Dispositivos (`devmgmt.msc`). Se um firmware anterior deixou o EXIO5 em CAN, a USB nativa some: use a porta UART ou o modo de download (abaixo). |
| `Failed to connect` / timeout na gravação | Entre em modo de download manualmente: segure **BOOT**, pressione e solte **RESET**, solte **BOOT**. Grave de novo e depois pressione RESET. |
| Porta ocupada (`could not open port`) | Feche outros monitores seriais (VS Code, PuTTY, SSCOM) e repita. |
| Build estranho após trocar versões | `idf.py fullclean` e compile de novo. Só limpa o build, não mexe na placa. |
| Tela preta após gravar | Pressione RESET. Confira os logs. Não presuma defeito sem comparar com o exemplo oficial. |

**Não use `idf.py erase-flash` como tentativa padrão.** Apagar a flash inteira raramente resolve problemas de gravação ou de tela e apaga NVS e calibrações. Só com motivo explícito.

Para voltar ao firmware de fábrica: o repositório oficial tem binários em `firmware/` (ex.: `lvgl_Porting.bin`). Veja https://docs.waveshare.com/ESP32-S3-Touch-LCD-7/Firmware-Flashing.

## 7. VS Code (opcional)

1. Instale a extensão **ESP-IDF** (`espressif.esp-idf-extension`).
2. Ela detecta a instalação pelo `C:\Espressif\tools\eim_idf.json`.
3. Command Palette → **ESP-IDF: Select Current ESP-IDF Version** → `v5.5.6`.
4. Abra a pasta do **projeto** (ex.: `projects/00-board-check`), não a raiz do repositório.

A extensão grava `.vscode/settings.json` com caminhos e porta locais. O `.gitignore` exclui `.vscode/`.
