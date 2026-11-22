# 6. Programming the ESP32

[Version française](../06-programmer-l-esp32.md)

## Tools

- **Arduino IDE** (or `arduino-cli`).
- The Espressif **ESP32 core** (tested with version **3.3.12**): install "esp32" in the boards manager.
- **Python 3** to generate the compressed web page.
- Board to select: **ESP32 Dev Module** (`esp32:esp32:esp32`).

The program only uses libraries supplied with the ESP32 core: `WiFi`, `ESPmDNS`, `ArduinoOTA`, `DNSServer`, `Preferences` and the `esp_http_server` server.

## The files

Everything is in [`firmware/porte_garage/`](../../firmware/porte_garage). Open `porte_garage.ino`: the IDE loads the other files of the folder.

| File | Role |
|---|---|
| `porte_garage.ino` | The logic: relay, Wi-Fi, passwords, history, light sensor, routes |
| `ServeurHttp.h` | Small adapter around the `esp_http_server` server |
| `page.h` | **Source** of the web page (HTML, CSS, JavaScript) |
| `page_gzip.h` | Compressed page, **generated** from `page.h` |
| `page_config.h` | Lightweight configuration page (no JavaScript) for the `ESP-Config` Wi-Fi |
| `icones.h` | App icons (PNG) |
| `generer_page.py` | Generates `page_gzip.h` |

See [`firmware/README.en.md`](../../firmware/README.en.md) for details of each part of the code.

## Rules to know

1. **Never write the HTML page in the `.ino`.** The Arduino tool inserts `#line` directives that break the JavaScript. The page is in `page.h`.
2. **After any change to `page.h`**, run `python firmware/porte_garage/generer_page.py` to regenerate `page_gzip.h`, then compile. The ESP32 sends the compressed page (about 10 KB).
3. **No password in the code.** The Wi-Fi and the secret code are set from the configuration page (guide 7) and kept in the ESP32 flash memory.

## First installation (by USB cable)

1. Connect the ESP32 through the **ESP32's USB-C port** (not the expansion board's).
2. In the IDE: board **ESP32 Dev Module**, the right serial port.
3. Close the **serial monitor** if it is open, otherwise the port is busy ("COM busy").
4. Generate the page: `python firmware/porte_garage/generer_page.py`.
5. Compile then upload. On the command line:

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/porte_garage
arduino-cli upload  --fqbn esp32:esp32:esp32 -p COM5 firmware/porte_garage
```

(Replace `COM5` with the port used.) The program takes about **82 %** of the flash.

**Check the compilation return code before uploading**: never start the upload if the compilation failed.

## Wireless updates (OTA)

As soon as a **secret code** is set, the ESP32 accepts updates over Wi-Fi. The **update password is the secret code**. The relay is **blocked during** an update.

- In the Arduino IDE: the network port appears under the name `esp-acces`.
- Or with the `espota.py` tool of the ESP32 core: address `esp-acces.local`, port `3232`, compiled binary, password = the secret code.

**Never type the code in a file of the repository.** On Windows, it can be kept in the Credential Manager and read by a small local script.

After an update, check:

```powershell
curl http://esp-acces.local/
```

The page must respond and the server must refuse `/etat` without a login.

## Pitfalls

- **ESP32 compiler on Windows**: depending on the environment, it may fail in a sandbox (error 623, DLL). Run it outside the sandbox.
- **Browser without gzip**: the server answers "406". All phone browsers accept gzip.
- **In scripts**, beware of `\n`: real line breaks inside a C string cause a compilation error.
- **Size**: above 90 % of the flash, OTA updates become difficult. Keep some margin.
