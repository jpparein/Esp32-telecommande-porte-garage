# 11. Security and limits

[Version française](../11-securite-et-limites.md)

## What is protected

| Subject | Measure |
|---|---|
| App access | Secret code (6 characters minimum) and first name. 128-bit `t` cookie, `HttpOnly`, `SameSite=Strict`, valid for 1 year. 8 phones at most. |
| Code guessing | **Constant-time** comparison; **5 wrong codes = 5-minute lockout**. |
| Code change | **All phones are forgotten** and must log in again. |
| Start-up | The **relay is set to rest** at the very beginning of `setup()`: no stray click on restart. |
| Pulse | 1 second, **minimum 3 s pause** between two commands. |
| OTA update | Enabled only if a code is set; password = secret code; **relay blocked** during the update. |
| Access from the Internet | None: home network only (access list of guide 8). |
| Door safety | The relay only **duplicates the manual switch**: the **safety photocell** stays active. |

## Known limits

1. **Unencrypted HTTP** on the home network: a third party already connected to the Wi-Fi can read or replay the traffic. Protect the Wi-Fi (WPA2/WPA3, strong password).
2. **Secret code stored in clear** in the ESP32 flash (it also serves as the OTA password). Someone holding the ESP32 can read it over USB.
3. **`ESP-Config` is an open Wi-Fi.** The settings are protected by the existing code, but the **very first setup** is open to the first comer. Do it at home, out of range of an outside scan.
4. **Without a position sensor**, the "open / closed" state is an **assumption** from the last request. The light detector reports manual movements but not their direction.
5. **A second press** while the lamp is already on is not detected.
6. If the **photocell** or the motorisation is faulty, **the assembly does not replace them**.

## Good practice

- Never connect the ESP32 ground to the control unit board; the relay alone makes the link.
- Reserve simulation mode for tests.
- Restrict the app to the home network; do not forward any port to the ESP32.
- For outside access, use a **VPN** (WireGuard, Tailscale) rather than opening a port.
- **Forgotten secret code**: the ESP32 must be reprogrammed by cable with a **flash erase** (`arduino-cli upload` with full erase, or `esptool erase_flash`), then the first setup redone.
