# The ESP32 program

[Version française](README.md)

Folder [`porte_garage/`](porte_garage). Board: **ESP32 Dev Module** (`esp32:esp32:esp32`), ESP32 core **3.3.12**. See [guide 6](../docs/en/06-programming-the-esp32.md) to compile and upload. Note: the program's on-screen texts and history labels are in French.

## Files

| File | Content |
|---|---|
| `porte_garage.ino` | All the logic (see below) |
| `ServeurHttp.h` | Adapter around `esp_http_server`: route table (32 maximum), cookie reading, POST bodies, gzip responses |
| `page.h` | **Source** of the main web page (HTML, CSS, JavaScript, WebP images in base64) |
| `page_gzip.h` | **Generated** by `generer_page.py` from `page.h`: never edit by hand |
| `page_config.h` | Lightweight Wi-Fi configuration page (no JavaScript, no gzip) for `ESP-Config` |
| `icones.h` | 192 and 512 PNG icons of the installed app |
| `generer_page.py` | Script generating `page_gzip.h` |

**Rule**: after any change to `page.h`, run `python generer_page.py` again, then compile. **Never** put the HTML page in the `.ino` (the Arduino tool inserts `#line` directives that break the JavaScript).

## Constants of `porte_garage.ino`

| Name | Value | Role |
|---|---|---|
| `PIN_BOOT` | 0 | BOOT button: 3 s = configuration mode |
| `PIN_LED_WIFI` | 2 | Board LED: steady = Wi-Fi OK, blinking = configuration |
| `PIN_RELAIS` | 27 | Relay command |
| `RELAIS_ACTIF` | `HIGH` | The KY-019 is active high; `RELAIS_REPOS` = the opposite |
| `ESSAIS_MAX` | 5 | Wrong codes before a 5-minute lockout |
| `NB_APPAREILS` | 8 | Phones remembered at most |
| `NB_HISTO` | 100 | Events kept in flash |
| `PIN_LUMIERE` | 34 | KY-018 light sensor (analog input, ADC1) |
| `SEUIL_LUMIERE` | 400 | Default sensitivity value (changeable in the app) |
| `FENETRE_ORDRE_S` | 30 | Default value of the "after a command" window |

## What the program does

**At start-up (`setup`)**: the relay is set to rest **first of all**. Then settings are read from flash. With no settings, or after BOOT 3 s: open `ESP-Config` Wi-Fi (AP+STA mode), DNS server and captive portal. Otherwise: connection to the home Wi-Fi, mDNS `esp-acces.local`, NTP time (Paris), HTTP server and OTA (if a code is set). `WiFi.setSleep(false)` for a fast response.

**Loop (`loop`)**:
- handles OTA and DNS, reads BOOT, blinks the LED;
- releases the relay pulse after 1 s;
- switches simulation mode off after 30 minutes;
- reads the light sensor every 0.1 s and detects a manual movement (guide 9).

## HTTP routes

| Route | Role | Login required |
|---|---|---|
| `/` | Main page (or configuration page for a request from `192.168.4.1`) | no |
| `/connexion` | Code and first name submission, cookie setting | no |
| `/etat` | Door state, simulation, time, name, etc. | yes |
| `/impulsion` | Gives the command (relay 1 s, minimum pause 3 s); `sens=ouverture|fermeture` is only for the history | yes |
| `/historique` | List of events (GET), clearing (POST) | yes |
| `/identite` | Phone first name | yes |
| `/simulation` | Simulation mode (GET/POST, RAM only) | yes |
| `/lumiere` | Light detector: live measurement and settings (GET/POST) | yes |
| `/oublier` | Forget a phone | yes |
| `/config` | Wi-Fi/code setup page | existing code required |
| `/reseaux`, `/reseaux-config` | Wi-Fi network search (asynchronous scan) | depends on page |
| `/wifi` | Saves the Wi-Fi and the code | existing code required |
| `/manifest.json`, `/icone192.png`, `/icone512.png` | Installation as an app | no |
| captive portal routes | `/generate_204`, `/hotspot-detect.html`, `/connecttest.txt`, etc. | no |

The limit is **32 routes** (`ServeurHttp.h`, `max_uri_handlers`). An extra route is silently ignored: when adding one, raise the limit.

## Settings in flash (`Preferences`)

| Space | Keys | Content |
|---|---|---|
| `porte` | `ssid`, `wp`, `mdp`, tokens, first names, `pj` | Wi-Fi, Wi-Fi password, secret code, remembered phones |
| `porte` | `sl`, `fo`, `la` | Sensor threshold, window after a command, detector enabled |
| `histo` | | 100 events (oldest → newest) |

## Light detector: the principle in a few lines

1. Read the ADC of D34 every 0.1 s.
2. Keep a **slow reference** (moving average); taken again after 10 minutes of false light.
3. "On" if `reference − value > threshold` **3 times in a row** (the program also handles the reverse direction).
4. If there was no app command in the previous `fo` seconds: write "Mouvement manuel (lumière N)" in the history.

## Security in the code

- **Constant-time** comparison of the code.
- 5 wrong codes = 5-minute lockout.
- `t` cookie: random 128-bit token, `HttpOnly`, `SameSite=Strict`, 1 year.
- Code change = all tokens erased.
- OTA only if a code is set; relay blocked during an update.

## Modifying the interface

1. First draw or adjust in [`../docs/demo/demo.html`](../docs/demo/demo.html) (example responses in a `<script id="demo">` block).
2. Once the interface is right, port it into `page.h` **removing this block** of example responses.
3. Run `python generer_page.py`, compile, upload.
4. Check the page: `curl http://esp-acces.local/`, then extract the `<script>` and pass it to `node --check`.

## Never put in this folder

Wi-Fi password, secret code, API keys: all of this is set from the configuration page and kept in flash.
