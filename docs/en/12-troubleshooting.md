# 12. Troubleshooting

[Version française](../12-depannage.md)

## The relay does not click, no LED

1. **Simulation mode** off? (orange banner = simulation: the relay does not move.)
2. Signal wire on the right row: **S of D27**, not D26 or D14.
3. Relay power: **+ on 5V** (not on V, which is 3.3 V), **− on GND**. Measure 4.6 to 5 V between + and −.
4. If the ESP32 switches off when touching a wire: short circuit. Unplug and check that two Dupont pins do not touch.
5. Test in isolation: connect the signal to another free pin to see whether the relay reacts.

## The relay clicks but the door does not move

- Check **continuity** between COM and NO when the relay is energised.
- Check the solder joints on C and D (guide 5) and their position on the **right legs** (the ones that beep when the button is pressed).
- Check that the motorisation is powered (circuit breaker).

## The sensor does not react

- It is on **D34**: S, V, G. The row just above is VN: it is not read by the program.
- In Settings, look at the **live measurement**; the value must change with the lighting.
- Threshold too high: lower it (guide 9).

## Cannot reach the ESP32

- Steady LED? If it blinks: configuration mode, connect to `ESP-Config`.
- LED off: Wi-Fi not connected. Hold **BOOT for 3 s** and redo the setup.
- Try the **IP address** instead of `esp-acces.local` (some phones handle mDNS badly).
- The router may have assigned another IP: create a **DHCP reservation**.

## "Too many wrong codes"

Wait 5 minutes (anti-guessing lockout). The lockout applies to all phones.

## The phone asks for the code again

A cookie belongs to one address: each phone must log in **once per address** (direct HTTP, HTTPS). Changing the secret code makes **all** phones forgotten.

## The history has no time (`--`)

The ESP32 has not yet received the NTP time (no Internet at start-up). The "Démarrage" line keeps `--`.

## The upload fails

- **"COM busy"**: close the serial monitor.
- **Compilation error 623** on Windows: run outside the sandbox.
- **OTA refused**: wrong password (it is the secret code) or the ESP32 is not on the network.
- **Page no longer displayed** after a change: `page.h` was modified without regenerating `page_gzip.h`.

## A manual movement is recorded as "manual" although it was the app

The "after a command" window is too short: increase it (guide 9).

## The ESP32 gets hot

Overheating indicates a fault: unplug, then check the wiring, especially that no 5 V wire goes to a 3.3 V signal pin.
