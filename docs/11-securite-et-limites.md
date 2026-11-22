# 11. Sécurité et limites

## Ce qui est protégé

| Sujet | Mesure |
|---|---|
| Accès à l'appli | Code secret (6 caractères minimum) et prénom. Cookie `t` de 128 bits, `HttpOnly`, `SameSite=Strict`, valable 1 an. 8 téléphones au maximum. |
| Devinette du code | Comparaison en **temps constant** ; **5 mauvais codes = blocage de 5 minutes**. |
| Changement de code | **Tous les téléphones sont oubliés** et doivent se reconnecter. |
| Démarrage | Le **relais est mis au repos** dès le début de `setup()` : pas de clic parasite au redémarrage. |
| Impulsion | 1 seconde, **pause mini de 3 s** entre deux ordres. |
| Mise à jour OTA | Activée seulement si un code est configuré ; mot de passe = code secret ; **relais bloqué** pendant la mise à jour. |
| Accès depuis Internet | Aucun : le réseau de la maison seulement (liste d'accès du guide 8). |
| Sécurité de la porte | Le relais ne fait que **doubler l'interrupteur manuel** : la **cellule de sécurité** reste active. |

## Limites connues

1. **HTTP non chiffré** sur le réseau de la maison : un tiers déjà connecté au Wi-Fi peut lire ou rejouer le trafic. Protéger le Wi-Fi (WPA2/WPA3, mot de passe fort).
2. **Code secret stocké en clair** dans la flash de l'ESP32 (il sert aussi de mot de passe OTA). Quelqu'un qui a l'ESP32 en main peut le lire par câble USB.
3. **`ESP-Config` est un Wi-Fi ouvert.** Les réglages sont protégés par le code existant, mais le **tout premier réglage** est ouvert au premier arrivé. À effectuer à domicile, hors de portée d'un scan extérieur.
4. **Sans capteur de position**, l'état « ouverte / fermée » est une **supposition** à partir de la dernière demande. Le détecteur de lumière signale les mouvements manuels mais pas leur sens.
5. **Un second appui** pendant que l'ampoule est déjà allumée n'est pas détecté.
6. Si la **cellule** ou la motorisation sont défectueux, **le montage ne les remplace pas**.

## Bonnes pratiques

- Ne jamais brancher la masse de l'ESP32 à la carte du boîtier de commande ; le relais seul fait le lien.
- Réserver le mode simulation aux essais.
- Limiter l'appli au réseau de la maison ; ne pas rediriger de port vers l'ESP32.
- Pour un accès extérieur, passer par un **VPN** (WireGuard, Tailscale) plutôt que d'ouvrir un port.
- **Oubli du code secret** : il faut reprogrammer l'ESP32 par câble en **effaçant la flash** (`arduino-cli upload` avec effacement complet, ou `esptool erase_flash`), puis refaire le premier réglage.
