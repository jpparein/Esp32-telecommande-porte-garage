# 12. Dépannage

[English version](en/12-troubleshooting.md)

## Le relais ne clique pas, aucune LED

1. **Mode simulation** désactivé ? (bandeau orange = simulation : le relais ne bouge pas.)
2. Fil de signal sur la bonne rangée : **S de D27**, pas D26 ni D14.
3. Alimentation du relais : **+ sur 5V** (pas sur V, qui est à 3,3 V), **− sur GND**. Mesurer 4,6 à 5 V entre + et −.
4. Si l'ESP32 s'éteint au contact d'un fil : court-circuit. Débrancher et vérifier que deux broches Dupont ne se touchent pas.
5. Tester en isolant : brancher le signal sur une autre broche libre pour voir si le relais réagit.

## Le relais clique mais la porte ne bouge pas

- Vérifier la **continuité** entre COM et NO quand le relais est actionné.
- Vérifier les soudures sur C et D (guide 5) et leur position sur les **bonnes pattes** (celles qui bipent à l'appui sur le bouton).
- Vérifier que la motorisation est alimentée (disjoncteur).

## Le capteur ne réagit pas

- Il est sur **D34** : S, V, G. La rangée située juste au-dessus est VN : elle n'est pas lue par le programme.
- Dans Réglages, consulter la **mesure en direct** ; la valeur doit varier à l'éclairage.
- Seuil trop haut : le baisser (guide 9).

## Impossible de joindre l'ESP32

- LED fixe ? Si elle clignote : mode configuration, se connecter à `ESP-Config`.
- LED éteinte : Wi-Fi non connecté. Maintenir **BOOT 3 s** et refaire le réglage.
- Essayer l'**adresse IP** au lieu de `esp-acces.local` (certains téléphones gèrent mal le mDNS).
- La box a peut-être attribué une autre IP : créer une **réservation DHCP**.

## « Trop de mauvais codes »

Attendre 5 minutes (blocage anti-devinette). Le blocage vaut pour tous les téléphones.

## Le téléphone redemande le code

Un cookie est propre à chaque adresse : chaque téléphone doit se connecter **une fois par adresse** (HTTP direct, HTTPS). Un changement du code secret fait oublier **tous** les téléphones.

## L'historique n'a pas l'heure (`--`)

L'ESP32 n'a pas encore reçu l'heure NTP (pas d'Internet au démarrage). La ligne « Démarrage » garde `--`.

## Le téléversement échoue

- **« COM busy »** : fermer le moniteur série.
- **Compilation en erreur 623** sous Windows : lancer hors bac à sable.
- **OTA refusé** : mauvais mot de passe (c'est le code secret) ou l'ESP32 n'est pas sur le réseau.
- **Page qui ne s'affiche plus** après une modification : `page.h` a été modifié sans régénérer `page_gzip.h`.

## Un mouvement manuel est noté « manuel » alors que c'était l'appli

La fenêtre « après un ordre » est trop courte : l'augmenter (guide 9).

## L'ESP32 chauffe

Une surchauffe indique un défaut : débrancher, puis vérifier les branchements, surtout qu'aucun fil 5 V ne va sur une broche de signal 3,3 V.
