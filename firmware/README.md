# Le programme de l'ESP32

[English version](README.en.md)

Dossier [`porte_garage/`](porte_garage). Carte : **ESP32 Dev Module** (`esp32:esp32:esp32`), cœur ESP32 **3.3.12**. Voir le [guide 6](../docs/06-programmer-l-esp32.md) pour compiler et envoyer.

## Fichiers

| Fichier | Contenu |
|---|---|
| `porte_garage.ino` | Toute la logique (voir plus bas) |
| `ServeurHttp.h` | Adaptateur autour de `esp_http_server` : table de routes (32 maximum), lecture du cookie, corps des POST, réponses gzip |
| `page.h` | **Source** de la page web principale (HTML, CSS, JavaScript, images WebP en base64) |
| `page_gzip.h` | **Généré** par `generer_page.py` à partir de `page.h` : ne jamais l'éditer à la main |
| `page_config.h` | Page de configuration Wi-Fi légère (sans JavaScript, sans gzip) pour `ESP-Config` |
| `icones.h` | Icônes PNG 192 et 512 de l'appli installée |
| `generer_page.py` | Script de génération de `page_gzip.h` |

**Règle** : après toute modification de `page.h`, relancer `python generer_page.py`, puis compiler. Ne **jamais** mettre la page HTML dans le `.ino` (l'outil Arduino y insère des `#line` qui cassent le JavaScript).

## Constantes de `porte_garage.ino`

| Nom | Valeur | Rôle |
|---|---|---|
| `PIN_BOOT` | 0 | Bouton BOOT : 3 s = mode configuration |
| `PIN_LED_WIFI` | 2 | LED de la carte : fixe = Wi-Fi OK, clignote = configuration |
| `PIN_RELAIS` | 27 | Commande du relais |
| `RELAIS_ACTIF` | `HIGH` | Le KY-019 est actif à l'état haut ; `RELAIS_REPOS` = l'inverse |
| `ESSAIS_MAX` | 5 | Mauvais codes avant blocage de 5 minutes |
| `NB_APPAREILS` | 8 | Téléphones mémorisés au maximum |
| `NB_HISTO` | 100 | Évènements gardés en flash |
| `PIN_LUMIERE` | 34 | Capteur de lumière KY-018 (entrée analogique, ADC1) |
| `SEUIL_LUMIERE` | 400 | Valeur par défaut de la sensibilité (modifiable dans l'appli) |
| `FENETRE_ORDRE_S` | 30 | Valeur par défaut de la fenêtre « après un ordre » |

## Ce que fait le programme

**Au démarrage (`setup`)** : le relais est mis **au repos en tout premier**. Puis lecture des réglages en flash. Sans réglage, ou après BOOT 3 s : Wi-Fi ouvert `ESP-Config` (mode AP+STA), serveur DNS et portail captif. Sinon : connexion au Wi-Fi de la maison, mDNS `esp-acces.local`, heure NTP (Paris), serveur HTTP et OTA (si un code est configuré). `WiFi.setSleep(false)` pour une réponse rapide.

**Boucle (`loop`)** :
- traite OTA et DNS, lit BOOT, fait clignoter la LED ;
- coupe l'impulsion du relais après 1 s ;
- coupe le mode simulation après 30 minutes ;
- lit le capteur de lumière toutes les 0,1 s et détecte un mouvement manuel (guide 9).

## Routes HTTP

| Route | Rôle | Connexion requise |
|---|---|---|
| `/` | Page principale (ou page de configuration pour une requête issue de `192.168.4.1`) | non |
| `/connexion` | Envoi du code et du prénom, pose du cookie | non |
| `/etat` | État de la porte, simulation, heure, nom, etc. | oui |
| `/impulsion` | Donne l'ordre (relais 1 s, pause mini 3 s) ; `sens=ouverture|fermeture` ne sert que pour l'historique | oui |
| `/historique` | Liste des évènements (GET), effacement (POST) | oui |
| `/identite` | Prénom du téléphone | oui |
| `/simulation` | Mode simulation (GET/POST, mémoire vive seulement) | oui |
| `/lumiere` | Détecteur de lumière : mesure en direct et réglages (GET/POST) | oui |
| `/oublier` | Oublier un téléphone | oui |
| `/config` | Page de réglage Wi-Fi/code | code existant exigé |
| `/reseaux`, `/reseaux-config` | Recherche des réseaux Wi-Fi (scan asynchrone) | selon page |
| `/wifi` | Enregistre le Wi-Fi et le code | code existant exigé |
| `/manifest.json`, `/icone192.png`, `/icone512.png` | Installation comme appli | non |
| routes de portail captif | `/generate_204`, `/hotspot-detect.html`, `/connecttest.txt`, etc. | non |

La limite est de **32 routes** (`ServeurHttp.h`, `max_uri_handlers`). Une route en trop est ignorée en silence : en cas d'ajout, augmenter la limite.

## Réglages en flash (`Preferences`)

| Espace | Clés | Contenu |
|---|---|---|
| `porte` | `ssid`, `wp`, `mdp`, jetons, prénoms, `pj` | Wi-Fi, mot de passe Wi-Fi, code secret, téléphones mémorisés |
| `porte` | `sl`, `fo`, `la` | Seuil du capteur, fenêtre après ordre, détecteur actif |
| `histo` | | 100 évènements (ancien → récent) |

## Détecteur de lumière : le principe en quelques lignes

1. Lire l'ADC de D34 toutes les 0,1 s.
2. Garder une **référence lente** (moyenne glissante) ; reprise après 10 minutes de fausse lumière.
3. « Allumée » si `référence − valeur > seuil` **3 fois de suite** (le programme gère aussi le sens inverse).
4. Si aucun ordre de l'appli dans les `fo` secondes précédentes : écrire « Mouvement manuel (lumière N) » dans l'historique.

## Sécurité dans le code

- Comparaison du code en **temps constant**.
- 5 mauvais codes = blocage 5 minutes.
- Cookie `t` : jeton aléatoire de 128 bits, `HttpOnly`, `SameSite=Strict`, 1 an.
- Changement de code = tous les jetons effacés.
- OTA seulement si un code est configuré ; relais bloqué pendant une mise à jour.

## Pour modifier l'interface

1. Dessiner ou ajuster d'abord dans [`../docs/demo/demo.html`](../docs/demo/demo.html) (réponses d'exemple dans un bloc `<script id="demo">`).
2. Une fois l'interface au point, la porter dans `page.h` **en retirant ce bloc** de réponses d'exemple.
3. Lancer `python generer_page.py`, compiler, envoyer.
4. Vérifier la page : `curl http://esp-acces.local/`, puis extraire le `<script>` et le passer à `node --check`.

## Ne jamais mettre dans ce dossier

Mot de passe Wi-Fi, code secret, clés d'API : tout cela se règle depuis la page de configuration et se garde dans la flash.
