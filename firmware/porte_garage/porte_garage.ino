/*
  Porte de garage : page web avec mot de passe, dans l'ESP32.

  Le telephone ouvre http://esp-acces.local (ou l'adresse IP affichee au demarrage).
  Premiere fois : tape le code secret et ton prenom. Le telephone est ensuite memorise (cookie) :
  plus besoin de le retaper. Le bouton fait coller le relais 1 seconde, comme un appui
  sur le bouton rouge du moteur. Le vrai bouton et la cellule de securite ne sont pas touches.

  Fonctions :
   - code secret une seule fois par telephone (appareil memorise, survit au redemarrage)
   - jusqu'a 8 appareils memorises ; bouton "Oublier tous les appareils"
   - historique des 100 derniers evenements, garde en flash (ordres, connexions, mots de passe faux)
   - blocage 5 minutes apres 5 mots de passe faux
   - pause mini de 3 secondes entre deux ordres

  Cablage du relais : S -> D27, + -> 5 V (VIN), - -> GND.

  Reglages (Wi-Fi, mot de passe) : AUCUN fichier a editer. Au premier demarrage l'ESP32 cree
  son propre Wi-Fi "ESP-Config" : connecte-toi dessus, ouvre http://192.168.4.1 et
  remplis le formulaire. Ensuite tout se change depuis la page (section Reglages).

  Mise a jour sans cable (OTA) : protegee par le code secret.
  BOOT maintenu 3 s : rouvre le Wi-Fi ESP-Config sans effacer les reglages.
  LED (GPIO2) : clignote en configuration, fixe si le Wi-Fi est connecte, eteinte sinon.

  Moniteur serie : 115200 bauds.
*/

#include <WiFi.h>
#include "ServeurHttp.h"
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <time.h>

// ---- Reglages ----
const int      PIN_BOOT = 0;
const int      PIN_LED_WIFI = 2; // LED D2 sur la carte
const int      PIN_RELAIS = 27;
const int      RELAIS_ACTIF = HIGH;               // module KY-019 : actif a l'etat haut
const int      RELAIS_REPOS = !RELAIS_ACTIF;
const uint32_t DUREE_IMPULSION_MS = 1000;         // temps pendant lequel le relais reste colle
const uint32_t PAUSE_MINI_MS = 3000;              // delai minimum entre deux impulsions
const int      ESSAIS_MAX = 5;                    // mauvais mots de passe avant blocage
const uint32_t BLOCAGE_MS = 5UL * 60UL * 1000UL;  // duree du blocage : 5 minutes
const char*    NOM_RESEAU = "esp-acces";
const uint32_t DUREE_SIMULATION_MS = 30UL * 60UL * 1000UL;   // le mode simulation se coupe seul
const int      NB_APPAREILS = 8;                 // telephones memorises au maximum
const int      NB_HISTO = 100;                    // evenements gardes
const int      PIN_LUMIERE = 34;                  // capteur de lumiere KY-018 pres de l'ampoule du moteur (entree analogique)
const int      SEUIL_LUMIERE = 400;               // valeur par defaut : ecart (sur 4095) qui signifie "ampoule allumee"
const int      FENETRE_ORDRE_S = 30;              // valeur par defaut : une lumiere dans les 30 s apres un ordre de l'appli est la notre
int            lumSeuil = SEUIL_LUMIERE;          // reglables depuis la page Reglages, gardes en flash
int            lumFenetre = FENETRE_ORDRE_S;
bool           lumActive = true;
volatile int   lumValeur = 0, lumReference = 0, lumEcart = 0;   // mesures en direct (pour l'etalonnage)
volatile bool  lumAllumee = false;

ServeurHttp serveur(80);
DNSServer dnsConfig;
volatile bool configApActif = false;
Preferences memoire;

portMUX_TYPE verrouRelais = portMUX_INITIALIZER_UNLOCKED;
volatile bool miseAJourEnCours = false;
volatile bool simulation = false;   // vrai : les ordres ne font pas cliquer le relais
uint32_t finSimulation = 0;
volatile bool     relaisColle = false;
volatile uint32_t finImpulsion = 0;
uint32_t derniereImpulsion = 0;
bool     impulsionDejaFaite = false;
int      essaisRates = 0;
bool     bloque = false;
uint32_t finBlocage = 0;

String   cfgSsid, cfgWifi, cfgMdp;   // reglages enregistres dans l'ESP32
bool     redemarrer = false;
uint32_t instantRedemarrage = 0;
String   jetons[NB_APPAREILS];     // appareils memorises (jeton aleatoire)
String   noms[NB_APPAREILS];       // prenom de chaque telephone memorise
int      prochainJeton = 0;

struct Evenement { uint32_t quand; char texte[40]; };
Evenement histo[NB_HISTO];
int       nbHisto = 0;
int       teteHisto = 0;

// La page web : source dans page.h, compressee dans page_gzip.h par generer_page.py (a relancer apres chaque modification de page.h).
#include "page_gzip.h"
#include "page_config.h"
#include "icones.h"

// ---------- Outils ----------

// Compare deux textes sans s'arreter au premier caractere different.
bool memeTexte(const String& a, const String& b) {
  uint8_t diff = (a.length() != b.length());
  size_t n = min(a.length(), b.length());
  for (size_t i = 0; i < n; i++) diff |= (a[i] ^ b[i]);
  return diff == 0;
}

void sauverHisto() {
  memoire.begin("histo", false);
  memoire.putBytes("h", histo, sizeof(histo));
  memoire.putInt("n", nbHisto);
  memoire.putInt("t", teteHisto);
  memoire.end();
}

void chargerHisto() {
  memoire.begin("histo", true);
  if (memoire.getBytesLength("h") == sizeof(histo)) {
    memoire.getBytes("h", histo, sizeof(histo));
    nbHisto = constrain(memoire.getInt("n", 0), 0, NB_HISTO);
    teteHisto = constrain(memoire.getInt("t", 0), 0, NB_HISTO - 1);
  }
  memoire.end();
}

void noter(const String& texte) {
  Evenement& e = histo[teteHisto];
  time_t t = time(nullptr);
  e.quand = (t > 1700000000) ? (uint32_t)t : 0;   // 0 = heure pas encore connue
  memset(e.texte, 0, sizeof(e.texte));
  texte.substring(0, sizeof(e.texte) - 1).toCharArray(e.texte, sizeof(e.texte));
  teteHisto = (teteHisto + 1) % NB_HISTO;
  if (nbHisto < NB_HISTO) nbHisto++;
  Serial.println(texte);
  sauverHisto();
}

String nouveauJeton() {
  String j;
  for (int i = 0; i < 4; i++) {
    char bloc[9];
    snprintf(bloc, sizeof(bloc), "%08x", (unsigned)esp_random());
    j += bloc;
  }
  return j;   // 32 caracteres hexadecimaux
}

void sauverJetons() {
  memoire.begin("porte", false);
  for (int i = 0; i < NB_APPAREILS; i++) { memoire.putString(("j" + String(i)).c_str(), jetons[i]); memoire.putString(("n" + String(i)).c_str(), noms[i]); }
  memoire.putInt("pj", prochainJeton);
  memoire.end();
}

void chargerJetons() {
  memoire.begin("porte", true);
  for (int i = 0; i < NB_APPAREILS; i++) { jetons[i] = memoire.getString(("j" + String(i)).c_str(), ""); noms[i] = memoire.getString(("n" + String(i)).c_str(), ""); }
  prochainJeton = memoire.getInt("pj", 0) % NB_APPAREILS;
  memoire.end();
}

bool refuseSiBloque();

void chargerReglages() {
  memoire.begin("porte", true);
  cfgSsid = memoire.getString("ssid", "");
  cfgWifi = memoire.getString("wp", "");
  cfgMdp  = memoire.getString("mdp", "");
  lumSeuil   = constrain(memoire.getInt("sl", SEUIL_LUMIERE), 50, 2000);
  lumFenetre = constrain(memoire.getInt("fo", FENETRE_ORDRE_S), 5, 120);
  lumActive  = memoire.getBool("la", true);
  memoire.end();
}

void sauverLumiere() {
  memoire.begin("porte", false);
  memoire.putInt("sl", lumSeuil);
  memoire.putInt("fo", lumFenetre);
  memoire.putBool("la", lumActive);
  memoire.end();
}

void sauverReglages() {
  memoire.begin("porte", false);
  memoire.putString("ssid", cfgSsid);
  memoire.putString("wp", cfgWifi);
  memoire.putString("mdp", cfgMdp);
  memoire.end();
}

// Verifie un mot de passe de la porte (avec compteur d'essais). Envoie l'erreur si besoin.
bool mdpBon(const String& essai) {
  if (refuseSiBloque()) return false;
  if (cfgMdp.length() == 0 || !memeTexte(essai, cfgMdp)) {
    essaisRates++;
    noter("Mot de passe faux " + String(essaisRates) + "/" + String(ESSAIS_MAX));
    if (essaisRates >= ESSAIS_MAX) {
      bloque = true;
      finBlocage = millis() + BLOCAGE_MS;
      noter("Blocage 5 minutes");
    }
    serveur.send(403, "text/plain; charset=utf-8", "Mot de passe faux.");
    return false;
  }
  essaisRates = 0;
  return true;
}

String jetonRecu();

String nettoyerNom(String n) {
  n.trim();
  String r;
  for (size_t i = 0; i < n.length(); i++) {
    char c = n[i];
    if (c == '<' || c == '>' || c == '&' || c == '"' || c == 39 || c == 92 || (uint8_t)c < 32) continue;
    if (r.length() < 14) r += c;
  }
  return r;
}

String appareil() {
  String jt = jetonRecu();
  if (jt.length() == 32) {
    for (int i = 0; i < NB_APPAREILS; i++) {
      if (jetons[i].length() == 32 && memeTexte(jetons[i], jt) && noms[i].length() > 0) return noms[i];
    }
  }
  String u = serveur.header("User-Agent");
  if (u.indexOf("iPhone") >= 0 || u.indexOf("iPad") >= 0) return "iPhone";
  if (u.indexOf("Android") >= 0) return "Android";
  if (u.indexOf("Windows") >= 0) return "PC";
  if (u.indexOf("Mac") >= 0) return "Mac";
  return "autre";
}

// Renvoie le jeton envoye par le telephone (cookie "t=..."), ou "".
String jetonRecu() {
  String c = serveur.header("Cookie");
  int i = c.indexOf("t=");
  while (i > 0 && c[i - 1] != ' ' && c[i - 1] != ';') i = c.indexOf("t=", i + 1);
  if (i < 0) return "";
  int fin = c.indexOf(';', i);
  return c.substring(i + 2, fin < 0 ? c.length() : fin);
}

bool appareilConnu() {
  String j = jetonRecu();
  if (j.length() != 32) return false;
  bool trouve = false;
  for (int i = 0; i < NB_APPAREILS; i++) {
    if (jetons[i].length() == 32 && memeTexte(jetons[i], j)) trouve = true;
  }
  return trouve;
}

// true si on est bloque (et envoie la reponse d'erreur).
bool refuseSiBloque() {
  if (!bloque) return false;
  if ((int32_t)(millis() - finBlocage) < 0) {
    serveur.send(429, "text/plain; charset=utf-8", "Trop d'essais. Reessaie dans quelques minutes.");
    return true;
  }
  bloque = false;
  essaisRates = 0;
  return false;
}

String heureTexte(const Evenement& e) {
  if (e.quand == 0) return String("--");
  time_t q = (time_t)e.quand;
  struct tm t;
  localtime_r(&q, &t);
  char b[24];
  strftime(b, sizeof(b), "%d/%m %H:%M:%S", &t);
  return String(b);
}

// ---------- Pages ----------

void pagePrincipale() {
  if (configApActif && serveur.header("Host").startsWith("192.168.4.1")) {
    String html = FPSTR(CONFIG_DEBUT);
    html += cfgMdp.length() == 0 ? FPSTR(CONFIG_NOUVEAU) : FPSTR(CONFIG_EXISTANT);
    html += FPSTR(CONFIG_FIN);
    serveur.send(200, "text/html; charset=utf-8", html);
    return;
  }
  serveur.sendGzip("text/html; charset=utf-8", PAGE_GZIP, sizeof(PAGE_GZIP));
}

void etat() {
  if (cfgMdp.length() == 0) serveur.send(200, "text/plain; charset=utf-8", "setup");
  else if (appareilConnu()) serveur.send(200, "text/plain; charset=utf-8", "ok");
  else serveur.send(401, "text/plain; charset=utf-8", "non");
}

void connexion() {
  if (!mdpBon(serveur.arg("mdp"))) return;

  String nom = nettoyerNom(serveur.arg("nom"));
  String j = nouveauJeton();
  jetons[prochainJeton] = j;
  noms[prochainJeton] = nom;
  prochainJeton = (prochainJeton + 1) % NB_APPAREILS;
  sauverJetons();
  noter("Connexion : " + (nom.length() ? nom : appareil()));

  serveur.sendHeader("Set-Cookie", "t=" + j + "; Max-Age=31536000; Path=/; HttpOnly; SameSite=Strict");
  serveur.send(200, "text/plain; charset=utf-8", "Connecte.");
}

// Mode simulation : garde en memoire vive seulement (disparait au redemarrage) et se coupe seul apres 30 minutes.
bool simulationActive() {
  if (simulation && (int32_t)(millis() - finSimulation) >= 0) simulation = false;
  return simulation;
}

String etatSimulation() {
  if (!simulationActive()) return "0";
  uint32_t reste = (finSimulation - millis()) / 60000UL + 1;
  return "1 " + String(reste);
}

void simulationPage() {
  if (!appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }
  if (serveur.estPost()) {
    bool actif = (serveur.arg("actif") == "1");
    if (actif != simulationActive()) {
      simulation = actif;
      finSimulation = millis() + DUREE_SIMULATION_MS;
      noter(String(actif ? "Simulation activee (" : "Simulation arretee (") + appareil() + ")");
    } else if (actif) {
      finSimulation = millis() + DUREE_SIMULATION_MS;   // reappui : on repart pour 30 minutes
    }
  }
  serveur.send(200, "text/plain; charset=utf-8", etatSimulation());
}

// Detecteur de lumiere : mesures en direct + reglages (seuil, delai, marche/arret).
void lumierePage() {
  if (!appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }
  if (serveur.estPost()) {
    int s = serveur.arg("seuil").toInt();
    int f = serveur.arg("fenetre").toInt();
    lumSeuil = constrain(s, 50, 2000);
    lumFenetre = constrain(f, 5, 120);
    lumActive = (serveur.arg("actif") == "1");
    sauverLumiere();
    noter("Detecteur de lumiere regle (" + appareil() + ")");
  }
  serveur.send(200, "text/plain; charset=utf-8",
    String(lumValeur) + " " + String(lumReference) + " " + String(lumEcart) + " " + String(lumSeuil) + " " +
    String(lumFenetre) + " " + String(lumActive ? 1 : 0) + " " + String(lumAllumee ? 1 : 0));
}

void impulsion() {
  if (refuseSiBloque()) return;
  if (!appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }

  uint32_t maintenant = millis();
  bool simu = simulationActive();   // simulation : tout se passe comme d'habitude, sauf le relais
  portENTER_CRITICAL(&verrouRelais);
  bool miseAJour = miseAJourEnCours;
  bool attendre = relaisColle || (impulsionDejaFaite && (maintenant - derniereImpulsion) < PAUSE_MINI_MS);
  if (!miseAJour && !attendre) {
    if (!simu) {
      finImpulsion = maintenant + DUREE_IMPULSION_MS;
      digitalWrite(PIN_RELAIS, RELAIS_ACTIF);
      relaisColle = true;
    }
    derniereImpulsion = maintenant;
    impulsionDejaFaite = true;
  }
  portEXIT_CRITICAL(&verrouRelais);
  if (miseAJour) { serveur.send(503, "text/plain; charset=utf-8", "Mise à jour en cours."); return; }
  if (attendre) { serveur.send(429, "text/plain; charset=utf-8", "Attends quelques secondes."); return; }
  String sens = serveur.arg("sens");
  noter((sens == "ouverture" ? "Ouverture (" : sens == "fermeture" ? "Fermeture (" : "Ordre envoye (") + appareil() + (simu ? ", simulation)" : ")"));
  serveur.send(200, "text/plain; charset=utf-8", simu ? "Simulation : relais non activé." : "Ordre envoyé.");
}

void historique() {
  if (!appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }
  String s;
  for (int k = 0; k < nbHisto; k++) {   // du plus recent au plus ancien
    int i = (teteHisto - 1 - k + NB_HISTO * 2) % NB_HISTO;
    s += heureTexte(histo[i]) + "  " + histo[i].texte + "\n";
  }
  if (s.length() == 0) s = "Rien pour l'instant.";
  serveur.send(200, "text/plain; charset=utf-8", s);
}

void effacerHistorique() {
  if (!appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }
  String qui = appareil();
  memset(histo, 0, sizeof(histo));
  nbHisto = 0;
  teteHisto = 0;
  noter("Historique efface (" + qui + ")");   // garde une trace de l'effacement
  serveur.send(200, "text/plain; charset=utf-8", "Historique effacé.");
}

void oublier() {
  if (!appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }
  for (int i = 0; i < NB_APPAREILS; i++) { jetons[i] = ""; noms[i] = ""; }
  prochainJeton = 0;
  sauverJetons();
  noter("Appareils oublies (" + appareil() + ")");
  serveur.sendHeader("Set-Cookie", "t=; Max-Age=0; Path=/");
  serveur.send(200, "text/plain; charset=utf-8", "Tous les telephones sont oublies.");
}

void config() {
  String ssid = serveur.arg("ssid"), wp = serveur.arg("wp"), mdp = serveur.arg("mdp");
  bool premier = (cfgMdp.length() == 0);

  if (!premier && !mdpBon(serveur.arg("actuel"))) return;

  if (premier && (ssid.length() == 0 || mdp.length() < 6)) {
    serveur.send(400, "text/plain; charset=utf-8", "Il faut le nom du Wi-Fi et un mot de passe de 6 caracteres mini.");
    return;
  }
  if (mdp.length() > 0 && mdp.length() < 6) {
    serveur.send(400, "text/plain; charset=utf-8", "Mot de passe de la porte : 6 caracteres mini.");
    return;
  }
  if (ssid.length() > 0) { cfgSsid = ssid; cfgWifi = wp; }   // nom et mot de passe Wi-Fi vont ensemble
  if (mdp.length() > 0) {
    cfgMdp = mdp;
    for (int i = 0; i < NB_APPAREILS; i++) { jetons[i] = ""; noms[i] = ""; }   // les telephones doivent se reconnecter
    prochainJeton = 0;
    sauverJetons();
  }
  sauverReglages();
  noter("Reglages changes (" + appareil() + ")");
  redemarrer = true;
  instantRedemarrage = millis() + 1500;
  serveur.send(200, "text/plain; charset=utf-8", "Enregistre. La porte redemarre (10 secondes), puis reouvre la page.");
}

// Service worker minimal : necessaire a Chrome (Android) pour proposer l'installation de l'application.
// Il ne garde rien en cache : tout passe par le boitier.
void serviceWorker() {
  serveur.send(200, "application/javascript; charset=utf-8",
    "self.addEventListener('install',function(){self.skipWaiting();});"
    "self.addEventListener('activate',function(e){e.waitUntil(self.clients.claim());});"
    "self.addEventListener('fetch',function(e){if(e.request.method!=='GET')return;"
    "e.respondWith(fetch(e.request).catch(function(){return new Response('Boitier injoignable.',{status:503,headers:{'Content-Type':'text/plain; charset=utf-8'}});}));});");
}

void manifeste() {
  serveur.send(200, "application/manifest+json; charset=utf-8",
    "{\"name\":\"Accès\",\"short_name\":\"Accès\",\"start_url\":\"/\",\"scope\":\"/\",\"display\":\"standalone\","
    "\"background_color\":\"#eef5f7\",\"theme_color\":\"#1469c3\","
    "\"icons\":[{\"src\":\"/icone192.png\",\"sizes\":\"192x192\",\"type\":\"image/png\"},"
    "{\"src\":\"/icone512.png\",\"sizes\":\"512x512\",\"type\":\"image/png\"}]}");
}

void reseaux() {
  if (cfgMdp.length() != 0 && !appareilConnu()) {
    serveur.send(401, "text/plain; charset=utf-8", "Connecte-toi d'abord.");
    return;
  }
  if (relaisColle) {
    serveur.send(429, "text/plain; charset=utf-8", "Attends la fin de l'ordre avant de chercher les Wi-Fi.");
    return;
  }
  int n = WiFi.scanNetworks();
  String s;
  for (int i = 0; i < n; i++) {
    String nom = WiFi.SSID(i);
    if (nom.length() == 0 || s.indexOf(nom + '\n') >= 0) continue;
    s += nom + '\n';
  }
  WiFi.scanDelete();
  if (s.length() == 0) s = "";
  serveur.send(200, "text/plain; charset=utf-8", s);
}

// Scan asynchrone : ne bloque pas le traitement des commandes du relais.
void reseauxConfig() {
  if (!configApActif || !serveur.header("Host").startsWith("192.168.4.1")) {
    serveur.send(404, "text/plain; charset=utf-8", "Page inconnue.");
    return;
  }
  int n = WiFi.scanComplete();
  if (n == WIFI_SCAN_FAILED) {
    int resultat = WiFi.scanNetworks(true, true);
    if (resultat == WIFI_SCAN_FAILED) {
      serveur.send(503, "text/plain; charset=utf-8", "Recherche indisponible.");
      return;
    }
    serveur.send(200, "text/plain; charset=utf-8", "BUSY");
    return;
  }
  if (n == WIFI_SCAN_RUNNING) {
    serveur.send(200, "text/plain; charset=utf-8", "BUSY");
    return;
  }
  String liste;
  for (int i = 0; i < n; ++i) {
    String nom = WiFi.SSID(i);
    if (nom.isEmpty() || nom == "ESP-Config" || nom.indexOf('\n') >= 0 || nom.indexOf('\r') >= 0) continue;
    if (liste.indexOf(nom + '\n') < 0) liste += nom + '\n';
  }
  WiFi.scanDelete();
  serveur.send(200, "text/plain; charset=utf-8", liste);
}

void introuvable() {
  serveur.send(404, "text/plain; charset=utf-8", "Page inconnue.");
}

// Le mode de configuration conserve les reglages et exige le code existant.
void activerConfig() {
  if (configApActif) return;
  WiFi.mode(WIFI_AP_STA);
  if (WiFi.softAP("ESP-Config")) {
    configApActif = true;
    dnsConfig.setErrorReplyCode(DNSReplyCode::NoError);
    dnsConfig.start(53, "*", WiFi.softAPIP());
    Serial.println("Configuration : ESP-Config, http://192.168.4.1");
  } else {
    Serial.println("Impossible de creer ESP-Config");
  }
}

void portailCaptif() {
  if (configApActif) serveur.redirect("http://192.168.4.1/");
  else serveur.send(404, "text/plain; charset=utf-8", "Page inconnue.");
}

// ---------- Programme ----------

void setup() {
  digitalWrite(PIN_RELAIS, RELAIS_REPOS);   // relais au repos avant tout
  pinMode(PIN_RELAIS, OUTPUT);
  digitalWrite(PIN_RELAIS, RELAIS_REPOS);
  pinMode(PIN_BOOT, INPUT_PULLUP);
  pinMode(PIN_LED_WIFI, OUTPUT);
  digitalWrite(PIN_LED_WIFI, LOW);

  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Porte de garage ===");

  chargerJetons();
  chargerReglages();
  chargerHisto();

  bool wifiOk = false;
  if (cfgSsid.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(NOM_RESEAU);
    WiFi.setSleep(false);                     // reponse plus rapide
    WiFi.begin(cfgSsid.c_str(), cfgWifi.c_str());
    Serial.print("Connexion au Wi-Fi");
    uint32_t debut = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - debut < 20000) {
      digitalWrite(PIN_LED_WIFI, (millis() / 500) % 2 ? HIGH : LOW);
      delay(300);
      Serial.print(".");
    }
    Serial.println();
    wifiOk = (WiFi.status() == WL_CONNECTED);
  }
  if (wifiOk) {
    Serial.print("Connecte. Adresse : http://");
    Serial.println(WiFi.localIP());
  } else {
    // Pas de reglage ou Wi-Fi introuvable : l'ESP32 cree son propre Wi-Fi pour le reglage.
    activerConfig();
    if (cfgSsid.length() > 0) WiFi.begin(cfgSsid.c_str(), cfgWifi.c_str());
  }

  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.google.com");   // heure de Paris

  if (MDNS.begin(NOM_RESEAU)) Serial.println("Nom : http://esp-acces.local");

  // OTA accessible uniquement si un code secret a ete configure.
  if (cfgMdp.length() >= 6) {
    ArduinoOTA.setHostname(NOM_RESEAU);
    ArduinoOTA.setMdnsEnabled(false); // mDNS est deja gere par le programme
    ArduinoOTA.setPassword(cfgMdp.c_str());
    ArduinoOTA.onStart([]() {
      portENTER_CRITICAL(&verrouRelais);
      miseAJourEnCours = true;
      digitalWrite(PIN_RELAIS, RELAIS_REPOS);
      relaisColle = false;
      portEXIT_CRITICAL(&verrouRelais);
      Serial.println("Mise a jour OTA en cours");
    });
    ArduinoOTA.onError([](ota_error_t) {
      portENTER_CRITICAL(&verrouRelais);
      miseAJourEnCours = false;
      portEXIT_CRITICAL(&verrouRelais);
      Serial.println("Echec de mise a jour OTA");
    });
    ArduinoOTA.begin();
  }


  const char* entetes[] = {"Cookie", "User-Agent"};
  serveur.collectHeaders(entetes, 2);

  serveur.begin();
  serveur.on("/", HTTP_GET, pagePrincipale);
  serveur.on("/etat", HTTP_GET, etat);
  serveur.on("/connexion", HTTP_POST, connexion);
  serveur.on("/impulsion", HTTP_POST, impulsion);
  serveur.on("/historique", HTTP_GET, historique);
  serveur.on("/identite", HTTP_GET, [](){ if (!appareilConnu()) { serveur.send(401, "text/plain", "non"); return; } serveur.send(200, "text/plain; charset=utf-8", appareil()); });
  serveur.on("/oublier", HTTP_POST, oublier);
  serveur.on("/effacer-historique", HTTP_POST, effacerHistorique);
  serveur.on("/simulation", HTTP_GET, simulationPage);
  serveur.on("/simulation", HTTP_POST, simulationPage);
  serveur.on("/lumiere", HTTP_GET, lumierePage);
  serveur.on("/lumiere", HTTP_POST, lumierePage);
  serveur.on("/config", HTTP_POST, config);
  serveur.on("/manifest.json", HTTP_GET, manifeste);
  serveur.on("/sw.js", HTTP_GET, serviceWorker);
  serveur.on("/icone192.png", HTTP_GET, [](){ serveur.sendBytes("image/png", ICONE_192, sizeof(ICONE_192)); });
  serveur.on("/icone512.png", HTTP_GET, [](){ serveur.sendBytes("image/png", ICONE_512, sizeof(ICONE_512)); });
  serveur.on("/reseaux", HTTP_GET, reseaux);
  serveur.on("/reseaux-config", HTTP_GET, reseauxConfig);
  serveur.on("/wifi", HTTP_GET, [](){ if (!appareilConnu()) { serveur.send(401, "text/plain", "non"); return; } serveur.send(200, "text/plain; charset=utf-8", WiFi.status()==WL_CONNECTED ? WiFi.SSID() : String("Non connecté")); });
  serveur.on("/generate_204", HTTP_GET, portailCaptif);
  serveur.on("/hotspot-detect.html", HTTP_GET, portailCaptif);
  serveur.on("/connecttest.txt", HTTP_GET, portailCaptif);


  noter("Demarrage");
}

void loop() {

  portENTER_CRITICAL(&verrouRelais);
  if (relaisColle && (int32_t)(millis() - finImpulsion) >= 0) {
    digitalWrite(PIN_RELAIS, RELAIS_REPOS);
    relaisColle = false;
  }
  portEXIT_CRITICAL(&verrouRelais);

  if (configApActif) dnsConfig.processNextRequest();

  static bool appuiBoot = false;
  static bool bootTraite = false;
  static uint32_t debutBoot = 0;
  if (digitalRead(PIN_BOOT) == LOW) {
    if (!appuiBoot) { appuiBoot = true; bootTraite = false; debutBoot = millis(); }
    if (!bootTraite && millis() - debutBoot >= 3000) {
      bootTraite = true;
      activerConfig();
    }
  } else {
    appuiBoot = false;
  }
  digitalWrite(PIN_LED_WIFI, configApActif ? ((millis() / 500) % 2 ? HIGH : LOW) : (WiFi.status() == WL_CONNECTED ? HIGH : LOW));

  if (cfgMdp.length() >= 6) ArduinoOTA.handle();

  // Capteur de lumiere : l'ampoule du moteur s'allume quand la porte bouge.
  // Si elle s'allume sans ordre recent de l'appli, c'est un mouvement manuel (bouton ou telecommande d'origine).
  static uint32_t dernierEchantillon = 0;
  static float    reference = -1;       // valeur "ampoule eteinte", suivie lentement
  static bool     lumiereAllumee = false;
  static int      nbDessus = 0;
  static uint32_t debutAllumee = 0;
  if (millis() - dernierEchantillon >= 100) {
    dernierEchantillon = millis();
    int somme = 0;
    for (int i = 0; i < 8; i++) somme += analogRead(PIN_LUMIERE);
    float valeur = somme / 8.0f;
    if (reference < 0) reference = valeur;
    float ecart = valeur - reference;
    if (ecart < 0) ecart = -ecart;
    lumValeur = (int)valeur; lumReference = (int)reference; lumEcart = (int)ecart; lumAllumee = lumiereAllumee;
    if (!lumiereAllumee) {
      if (ecart > lumSeuil) {
        if (++nbDessus >= 3) {          // 3 mesures de suite (0,3 s) : on est sur
          lumiereAllumee = true;
          debutAllumee = millis();
          bool notre = impulsionDejaFaite && (millis() - derniereImpulsion) < (uint32_t)lumFenetre * 1000UL;
          if (lumActive && !notre && millis() > 5000) noter("Mouvement manuel (lumiere " + String((int)ecart) + ")");
        }
      } else {
        nbDessus = 0;
        reference += (valeur - reference) * 0.02f;   // suit doucement le jour qui change
      }
    } else if (ecart < lumSeuil / 2) {
      lumiereAllumee = false;
      nbDessus = 0;
    } else if (millis() - debutAllumee > 600000UL) {
      // allumee depuis plus de 10 min : ce n'est pas l'ampoule, la lumiere ambiante a change (capteur deplace, jour...)
      reference = valeur;
      lumiereAllumee = false;
      nbDessus = 0;
    }
  }

  if (redemarrer && (int32_t)(millis() - instantRedemarrage) >= 0) ESP.restart();

  static uint32_t dernierControle = 0;
  if (millis() - dernierControle > 10000) {
    dernierControle = millis();
    if (cfgSsid.length() > 0 && WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi perdu, reconnexion...");
      WiFi.reconnect();
    }
  }
}
