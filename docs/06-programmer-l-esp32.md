# 6. Programmer l'ESP32

[English version](en/06-programming-the-esp32.md)

## Outils

- **Arduino IDE** (ou `arduino-cli`).
- Le **cœur ESP32** d'Espressif (testé avec la version **3.3.12**) : dans le gestionnaire de cartes, installer « esp32 ».
- **Python 3** pour générer la page web compressée.
- Carte à choisir : **ESP32 Dev Module** (`esp32:esp32:esp32`).

Le programme n'utilise que des bibliothèques fournies avec le cœur ESP32 : `WiFi`, `ESPmDNS`, `ArduinoOTA`, `DNSServer`, `Preferences` et le serveur `esp_http_server`.

## Les fichiers

Tout est dans [`firmware/porte_garage/`](../firmware/porte_garage). Ouvrir `porte_garage.ino` : l'IDE charge les autres fichiers du dossier.

| Fichier | Rôle |
|---|---|
| `porte_garage.ino` | La logique : relais, Wi-Fi, mots de passe, historique, capteur de lumière, routes |
| `ServeurHttp.h` | Petit adaptateur autour du serveur `esp_http_server` |
| `page.h` | **Source** de la page web (HTML, CSS, JavaScript) |
| `page_gzip.h` | Page compressée, **générée** à partir de `page.h` |
| `page_config.h` | Page de configuration légère (sans JavaScript) du Wi-Fi `ESP-Config` |
| `icones.h` | Icônes de l'appli (PNG) |
| `generer_page.py` | Génère `page_gzip.h` |

Voir [`firmware/README.md`](../firmware/README.md) pour le détail de chaque partie du code.

## Règles à connaître

1. **Ne jamais écrire la page HTML dans le `.ino`.** L'outil Arduino y insère des directives `#line` qui cassent le JavaScript. La page est dans `page.h`.
2. **Après toute modification de `page.h`**, lancer `python firmware/porte_garage/generer_page.py` pour régénérer `page_gzip.h`, puis compiler. L'ESP32 envoie la page compressée (environ 10 Ko).
3. **Aucun mot de passe dans le code.** Le Wi-Fi et le code secret se règlent depuis la page de configuration (guide 7) et sont gardés dans la mémoire flash de l'ESP32.

## Première installation (par câble USB)

1. Brancher l'ESP32 par le **port USB-C de l'ESP32** (pas celui de la plaque d'extension).
2. Dans l'IDE : carte **ESP32 Dev Module**, le bon port série.
3. Fermer le **moniteur série** s'il est ouvert, sinon le port est occupé (« COM busy »).
4. Générer la page : `python firmware/porte_garage/generer_page.py`.
5. Compiler puis envoyer. En ligne de commande :

```powershell
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/porte_garage
arduino-cli upload  --fqbn esp32:esp32:esp32 -p COM5 firmware/porte_garage
```

(Remplacer `COM5` par le port utilisé.) Le programme occupe environ **82 %** de la flash.

**Vérifier le code de retour de la compilation avant l'envoi** : ne jamais lancer l'envoi si la compilation a échoué.

## Mises à jour sans câble (OTA)

Dès qu'un **code secret** est configuré, l'ESP32 accepte les mises à jour par Wi-Fi. Le **mot de passe de mise à jour est le code secret**. Le relais est **bloqué pendant** une mise à jour.

- Dans l'IDE Arduino : le port réseau apparaît sous le nom `esp-acces`.
- Ou avec l'outil `espota.py` du cœur ESP32 : adresse `esp-acces.local`, port `3232`, binaire compilé, mot de passe = le code secret.

**Ne jamais saisir le code dans un fichier du dépôt.** Sous Windows, il peut être conservé dans le Gestionnaire d'identification puis lu par un petit script local.

Après une mise à jour, vérifier :

```powershell
curl http://esp-acces.local/
```

La page doit répondre et le serveur doit refuser `/etat` sans connexion.

## Pièges

- **Compilateur ESP32 sous Windows** : selon l'environnement, il peut échouer dans un bac à sable (erreur 623, DLL). Le lancer hors du bac à sable.
- **Navigateur sans gzip** : le serveur répond « 406 ». Tous les navigateurs de téléphone acceptent gzip.
- **Dans les scripts**, attention aux `\n` : de vrais retours à la ligne dans une chaîne C provoquent une erreur de compilation.
- **Taille** : au-delà de 90 % de la flash, les mises à jour OTA deviennent difficiles. Prévoir de la marge.
