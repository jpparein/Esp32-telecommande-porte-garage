#pragma once
#include <pgmspace.h>

const char PAGE[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="fr"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="color-scheme" content="light dark">
<title>Accès</title>
<meta name="theme-color" content="#1469c3">
<link rel="manifest" href="/manifest.json">
<link rel="apple-touch-icon" href="/icone192.png">
<link rel="icon" type="image/png" sizes="192x192" href="/icone192.png">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-title" content="Accès">
<style>
:root{--bg:#dce8ef;--bg2:#edf5f8;--panel:#fff;--panel2:#f1f7fa;--bord:#c3d6e0;--txt:#152b3c;--doux:#5b7686;--acc:#3d83b0;--acc2:#2f6d99;--ok:#1f9d6b;--ok2:#17805a;--warn:#c97a00;--warnbg:#fff1d6;--err:#d33f55;--nav:#fffffff2;--mur1:#3a5265;--mur2:#1b3042;--cadre:#6b8796;--int:#0a1722;--m1:#dbe9ee;--m2:#8aa5b1;--sol:#7e9aa7;--sh:0 14px 36px rgba(42,78,103,.16)}
body.dark{--bg:#0a1322;--bg2:#12213a;--panel:#15233a;--panel2:#1b2c47;--bord:#2a3d58;--txt:#eaf2fb;--doux:#93a9bf;--acc:#5aa6d6;--acc2:#4891c0;--ok:#35c48c;--ok2:#25a574;--warn:#f0a63a;--warnbg:#3a2c10;--err:#f06a7e;--nav:#0f1c32f2;--sh:0 14px 36px rgba(0,0,0,.4)}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
html{-webkit-text-size-adjust:100%}
body{margin:0;min-height:100vh;min-height:100dvh;font-family:system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;color:var(--txt);background:linear-gradient(180deg,var(--bg2),var(--bg)) fixed}
.cache{display:none!important}
main{width:100%;max-width:520px;margin:0 auto;padding:calc(18px + env(safe-area-inset-top)) 18px calc(104px + env(safe-area-inset-bottom))}
.centre{min-height:100vh;min-height:100dvh;display:flex;flex-direction:column;justify-content:center;max-width:460px;margin:0 auto;padding:24px 18px}
header{display:flex;align-items:center;justify-content:space-between;margin-bottom:6px}
.brand{font-size:13px;font-weight:800;letter-spacing:.18em;color:var(--acc2)}
.brand i{display:inline-block;width:9px;height:9px;border-radius:50%;background:var(--ok);margin-right:9px;box-shadow:0 0 0 4px rgba(31,157,107,.18)}
h1{font-size:30px;letter-spacing:-.03em;line-height:1.1;margin:8px 0 4px}
h2{font-size:17px;margin:0 0 8px}
p{color:var(--doux);line-height:1.5;margin:0 0 12px}
.sous{margin:0 0 18px}
.panel{background:var(--panel);border:1px solid var(--bord);border-radius:22px;box-shadow:var(--sh);overflow:hidden}
.pad{padding:18px}
.gap{margin-top:14px}
.cote{margin-top:18px}
/* simulation */
#sim{position:fixed;top:0;left:0;right:0;z-index:20;background:var(--warn);color:#fff;text-align:center;font-weight:800;font-size:14px;padding:calc(8px + env(safe-area-inset-top)) 12px 8px;letter-spacing:.02em}
body.sim main{padding-top:calc(58px + env(safe-area-inset-top))}
body.sim .porte-carte{border:2px dashed var(--warn)}
/* porte */
.scene{padding:20px 22px 6px}
.scene svg{display:block;width:100%;max-width:360px;margin:0 auto;height:auto}
.mur-a{stop-color:var(--mur1)}.mur-b{stop-color:var(--mur2)}.m-a{stop-color:var(--m1)}.m-b{stop-color:var(--m2)}
.cadre{fill:url(#mur);stroke:var(--cadre);stroke-width:3}.int{fill:var(--int)}
.lame{fill:url(#metal);stroke:var(--cadre);stroke-width:2}.poignee{fill:var(--cadre)}
.solrect{stroke:var(--sol);stroke-width:6;stroke-linecap:round}
.lames{transition:transform 1.6s cubic-bezier(.4,0,.2,1)}
.lames.open{transform:translateY(-118px)}
.status{display:flex;align-items:center;justify-content:space-between;gap:10px;padding:14px 20px 16px;border-top:1px solid var(--bord)}
.status b{display:block;font-size:16px}.status small{color:var(--doux)}
.pill.sim{background:rgba(201,122,0,.16);color:var(--warn)}
.pill{font-size:13px;font-weight:700;padding:7px 12px;border-radius:30px;background:rgba(31,157,107,.14);color:var(--ok2);white-space:nowrap}
.actions{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.action{display:flex;align-items:center;justify-content:center;gap:10px;border:0;border-radius:18px;padding:18px 10px;font:inherit;font-size:19px;font-weight:800;color:#fff;cursor:pointer;box-shadow:0 10px 24px rgba(0,0,0,.16);transition:transform .12s}
.action span{font-size:24px;line-height:1}
.action:active{transform:scale(.97)}
.ouvrir{background:linear-gradient(135deg,#34c08a,var(--ok2))}
.fermer{background:linear-gradient(135deg,#5aa6d6,var(--acc2))}
.action.grise{background:var(--panel2);color:var(--doux);box-shadow:none;border:1px dashed var(--bord)}
.lien.danger{color:var(--err);border-color:var(--err)}
.lien{display:block;width:100%;margin-top:12px;padding:13px;border-radius:14px;border:1px solid var(--bord);background:var(--panel);color:var(--txt);font:inherit;font-weight:700;cursor:pointer}
.caption{font-size:13px;text-align:center;margin:12px 0 0}
.dern{display:flex;align-items:center;gap:13px;padding:15px 16px}
.ico{width:42px;height:42px;border-radius:13px;display:flex;align-items:center;justify-content:center;font-size:21px;font-weight:800;flex:none;background:rgba(61,131,176,.14);color:var(--acc2)}
.ico.o{background:rgba(31,157,107,.14);color:var(--ok2)}.ico.err{background:rgba(211,63,85,.14);color:var(--err)}
.dern strong,.item strong{display:block;font-size:15px}.dern small,.item small{color:var(--doux);font-size:13px}
.titre-bloc{font-size:13px;font-weight:800;letter-spacing:.12em;text-transform:uppercase;color:var(--doux);margin:20px 4px 8px}
/* historique */
.liste{max-height:58vh;max-height:58dvh;overflow:auto;-webkit-overflow-scrolling:touch}
.item{display:flex;align-items:center;gap:12px;padding:13px 16px;border-bottom:1px solid var(--bord)}
.item:last-child{border:0}.item>div:nth-child(2){flex:1;min-width:0}
.item time{font-size:12px;color:var(--doux);white-space:nowrap}
/* reglages */
.ligne{display:flex;align-items:center;justify-content:space-between;gap:14px}
.ligne p{margin:4px 0 0}
.themes{display:grid;grid-template-columns:repeat(3,1fr);gap:8px;margin-top:10px}
.themes button{padding:12px;border-radius:12px;border:1px solid var(--bord);background:var(--panel2);color:var(--txt);font:inherit;font-weight:700;cursor:pointer}
.themes button.sel{background:var(--acc);border-color:var(--acc);color:#fff}
.sw{position:relative;width:58px;height:34px;flex:none}
.sw input{opacity:0;position:absolute;inset:0;width:100%;height:100%;margin:0;cursor:pointer;z-index:2}
.sw i{position:absolute;inset:0;border-radius:20px;background:var(--bord);transition:.2s}
.sw i:after{content:'';position:absolute;top:4px;left:4px;width:26px;height:26px;border-radius:50%;background:#fff;transition:.2s;box-shadow:0 2px 5px rgba(0,0,0,.3)}
.sw input:checked+i{background:var(--warn)}.sw input:checked+i:after{transform:translateX(24px)}
details{border-top:1px solid var(--bord);padding-top:12px;margin-top:12px}
summary{font-weight:800;cursor:pointer;font-size:16px;list-style:none;display:flex;justify-content:space-between;align-items:center}
summary::-webkit-details-marker{display:none}summary:after{content:'+';font-size:22px;color:var(--doux)}details[open] summary:after{content:'−'}
details>*:not(summary){margin-top:12px}
label{display:block;font-size:14px;font-weight:700;margin:0 0 6px}label small{font-weight:400;color:var(--doux)}
input[type=text],input[type=password],input[type=number],select{width:100%;font:inherit;font-size:17px;padding:13px;margin:0 0 14px;border-radius:12px;border:1px solid var(--bord);background:var(--panel2);color:var(--txt);outline:none}
input:focus,select:focus{border-color:var(--acc)}
.btn{display:block;width:100%;font:inherit;font-size:17px;font-weight:800;padding:15px;border:0;border-radius:14px;color:#fff;background:linear-gradient(135deg,#5aa6d6,var(--acc2));cursor:pointer}
.btn.gris{background:var(--panel2);color:var(--txt);border:1px solid var(--bord);margin-bottom:14px;font-weight:700}
.btn.rouge{background:linear-gradient(135deg,#ef7a8b,var(--err))}
.led{font-size:14px;color:var(--doux);line-height:1.7}
/* menu */
nav{position:fixed;left:0;right:0;bottom:0;background:var(--nav);border-top:1px solid var(--bord);padding:8px 10px calc(8px + env(safe-area-inset-bottom));z-index:10;-webkit-backdrop-filter:blur(8px);backdrop-filter:blur(8px)}
nav div{max-width:520px;margin:0 auto;display:grid;grid-template-columns:repeat(3,1fr);gap:8px}
nav button{display:flex;flex-direction:column;align-items:center;gap:3px;padding:8px 0;border:0;border-radius:14px;background:none;color:var(--doux);font:inherit;font-size:12px;font-weight:700;cursor:pointer}
nav button span{font-size:22px;line-height:1}nav button.active{background:rgba(61,131,176,.14);color:var(--acc2)}
/* feuille et toast */
#voile{position:fixed;inset:0;background:rgba(8,18,30,.55);z-index:40;display:flex;align-items:flex-end;justify-content:center}
#feuille{background:var(--panel);width:100%;max-width:520px;border-radius:26px 26px 0 0;padding:26px 20px calc(24px + env(safe-area-inset-bottom));text-align:center}
#feuille h3{margin:0 0 6px;font-size:21px}#feuille .btn+.btn{margin-top:10px}
#toast{position:fixed;left:50%;bottom:calc(104px + env(safe-area-inset-bottom));transform:translate(-50%,20px);background:#26384a;color:#fff;padding:12px 20px;border-radius:30px;font-size:15px;font-weight:600;opacity:0;transition:.25s;z-index:50;max-width:90%;text-align:center;pointer-events:none}
#toast.vu{opacity:1;transform:translate(-50%,0)}#toast.ok{background:var(--ok2)}#toast.err{background:#b8283f}
.logo{width:72px;height:72px;border-radius:22px;background:linear-gradient(135deg,#5aa6d6,var(--acc2));display:flex;align-items:center;justify-content:center;color:#fff;font-size:34px;margin:0 auto 14px}
.ent{text-align:center;margin-bottom:18px}
/* tablette et paysage */
@media (min-width:700px){main{max-width:640px}nav div{max-width:640px}#feuille{max-width:560px;border-radius:26px;margin-bottom:24px}#voile{align-items:center}h1{font-size:34px}}
@media (orientation:landscape) and (max-height:560px),(orientation:landscape) and (min-width:900px){
  main{max-width:940px}nav div{max-width:940px}
  .grille{display:grid;grid-template-columns:1.15fr 1fr;gap:18px;align-items:start}
  .grille .porte-carte{margin:0}.grille .cote{margin-top:0}
  .scene{padding:14px 18px 4px}.scene svg{max-width:300px}
  .liste{max-height:62vh}
  #voile{align-items:center}#feuille{border-radius:26px}
}
@media (max-width:360px){h1{font-size:26px}.action{font-size:17px;padding:16px 6px}}
</style></head><body>

<div id="sim" class="cache">MODE SIMULATION : la porte ne bouge pas</div>

<!-- INSTALLATION -->
<div id="ec-setup" class="centre cache">
  <div class="ent"><div class="logo">⚙</div><h1>Installation</h1><p>3 petites étapes, une seule fois.</p></div>
  <div class="panel pad">
    <h2>Étape 1 : le Wi-Fi de la maison</h2>
    <p>C'est le Wi-Fi de ta box, celui de ton téléphone chez toi.</p>
    <button class="btn gris" onclick="chercher('s1','ls1')">Chercher les Wi-Fi autour</button>
    <select id="ls1" class="cache" onchange="$('s1').value=this.value"></select>
    <label for="s1">Nom du Wi-Fi</label><input id="s1" type="text" autocapitalize="none" autocomplete="off">
    <label for="s2">Mot de passe de ce Wi-Fi</label><input id="s2" type="password">
  </div>
  <div class="panel pad gap">
    <h2>Étape 2 : ton code secret</h2>
    <p>Tu l'inventes. Il sert à ouvrir la porte. Tu le tapes une seule fois sur chaque téléphone. 6 caractères minimum.</p>
    <label for="s3">Code secret</label><input id="s3" type="password">
    <label for="s4">Retape le code secret</label><input id="s4" type="password">
  </div>
  <button class="btn gap" onclick="installer()">Étape 3 : enregistrer</button>
</div>

<!-- CONNEXION -->
<div id="ec-login" class="centre cache">
  <div class="ent"><div class="logo">🔒</div><h1>Code secret</h1><p>Tape ton code une seule fois : ce téléphone s'en souviendra.</p></div>
  <div class="panel pad">
    <label for="nom">Ton prénom <small>(il apparaît dans l'historique)</small></label>
    <input id="nom" type="text" maxlength="14" autocomplete="given-name">
    <label for="p">Code secret</label>
    <input id="p" type="password" autocomplete="current-password" onkeydown="if(event.key=='Enter')entrer()">
    <button class="btn" onclick="entrer()">Se connecter</button>
  </div>
</div>

<!-- APPLICATION -->
<div id="ec-app" class="cache">
<main>

<section id="porte">
  <header><span class="brand"><i></i>ACCÈS</span></header>
  <h1>Garage</h1><p class="sous">Commandez votre porte.</p>
  <div class="grille">
    <div class="panel porte-carte">
      <div class="scene">
        <svg viewBox="0 0 320 214" role="img" aria-label="Animation illustrative de la porte">
          <defs>
            <linearGradient id="mur" x2="1" y2="1"><stop offset="0" class="mur-a"/><stop offset="1" class="mur-b"/></linearGradient>
            <linearGradient id="metal" x2="0" y2="1"><stop offset="0" class="m-a"/><stop offset="1" class="m-b"/></linearGradient>
            <clipPath id="cut"><rect x="48" y="36" width="224" height="158" rx="2"/></clipPath>
          </defs>
          <rect class="cadre" x="27" y="18" width="266" height="186" rx="12"/>
          <rect class="int" x="41" y="29" width="238" height="169" rx="5"/>
          <g clip-path="url(#cut)"><g id="lames" class="lames">
            <rect class="lame" x="48" y="36" width="224" height="38" rx="3"/>
            <rect class="lame" x="48" y="76" width="224" height="38" rx="3"/>
            <rect class="lame" x="48" y="116" width="224" height="38" rx="3"/>
            <rect class="lame" x="48" y="156" width="224" height="38" rx="3"/>
            <rect class="poignee" x="148" y="177" width="24" height="5" rx="3"/>
          </g></g>
          <path class="solrect" d="M18 207h284"/>
        </svg>
      </div>
      <div class="status"><div><b>Porte du garage</b><small id="posinfo">Position réelle inconnue</small></div><span class="pill" id="pill">● Prête</span></div>
    </div>

    <div class="cote">
      <div class="actions">
        <button id="b-ouvrir" class="action ouvrir" onclick="demander('ouverture')"><span>↑</span> Ouvrir</button>
        <button id="b-fermer" class="action fermer" onclick="demander('fermeture')"><span>↓</span> Fermer</button>
      </div>
      <button id="forcer" class="lien cache" onclick="forcer()">Forcer la dernière commande</button>
      <p class="caption">L'animation illustre la commande, pas la position réelle.</p>
      <div class="titre-bloc">Dernière activité</div>
      <div class="panel dern"><div class="ico" id="d-ico">•</div><div><strong id="d-lib">Aucune activité</strong><small id="d-det"></small></div></div>
    </div>
  </div>
</section>

<section id="histo" class="cache">
  <header><span class="brand"><i></i>ACCÈS</span></header>
  <h1>Historique</h1><p class="sous">Les dernières activités, la plus récente en haut.</p>
  <div class="panel liste" id="liste"></div>
  <button class="lien" onclick="exporter()">↓ &nbsp;Exporter en fichier (Excel)</button>
  <button class="lien danger" onclick="effacer()">Effacer l'historique</button>
  <p class="caption">Jusqu'à 100 événements conservés.</p>
</section>

<section id="regl" class="cache">
  <header><span class="brand"><i></i>ACCÈS</span></header>
  <h1>Réglages</h1><p class="sous">Ce téléphone et le boîtier.</p>

  <div class="panel pad">
    <h2>Apparence</h2>
    <p style="margin:0">Pour ce téléphone seulement.</p>
    <div class="themes"><button id="th-clair" onclick="theme('clair')">Clair</button><button id="th-sombre" onclick="theme('sombre')">Sombre</button><button id="th-auto" onclick="theme('auto')">Auto</button></div>
  </div>

  <div class="panel pad gap">
    <div class="ligne">
      <div><h2 style="margin:0">Mode simulation</h2><p>Pour tester l'application : la porte ne bouge pas. Se coupe tout seul après 30 minutes.</p></div>
      <label class="sw"><input type="checkbox" id="simcase" onchange="basculerSim(this.checked)"><i></i></label>
    </div>
  </div>

  <div class="panel pad gap">
    <div class="ligne">
      <div><h2 style="margin:0">Détecteur de mouvement manuel</h2><p>Un capteur de lumière près de l’ampoule du moteur repère quand la porte bouge sans ordre de l’appli (bouton ou télécommande d’origine). L’historique note alors « Mouvement manuel ».</p></div>
      <label class="sw"><input type="checkbox" id="lum-actif" onchange="enregistrerLumiere()"><i></i></label>
    </div>
    <p>Mesure en direct : <strong id="lum-v">…</strong> · référence <strong id="lum-r">…</strong> · écart <strong id="lum-e">…</strong> · <strong id="lum-a">…</strong></p>
    <details>
      <summary>Étalonnage</summary>
      <p>1. Regarde l’« écart » ci-dessus ampoule éteinte (proche de 0). 2. Fais bouger la porte avec le bouton : l’écart monte tant que l’ampoule est allumée. 3. Mets la sensibilité à peu près à la moitié de cet écart, puis enregistre.</p>
      <label for="lum-seuil">Sensibilité <small>(écart à partir duquel on dit « ampoule allumée », 50 à 2000 ; plus petit = plus sensible)</small></label><input id="lum-seuil" type="number" inputmode="numeric" min="50" max="2000">
      <label for="lum-fen">Délai après un ordre de l’appli <small>(secondes, 5 à 120 : la lumière qui s’allume pendant ce temps vient de l’appli, pas d’un mouvement manuel)</small></label><input id="lum-fen" type="number" inputmode="numeric" min="5" max="120">
      <button class="btn" onclick="enregistrerLumiere()">Enregistrer le détecteur</button>
    </details>
  </div>

  <div class="panel pad gap">
    <h2>Connexion du boîtier</h2>
    <p style="margin:0">Wi-Fi connecté : <strong id="wifi-actuel">…</strong></p>
    <details>
      <summary>Changer de Wi-Fi</summary>
      <p>À faire si tu changes de box ou le mot de passe du Wi-Fi. Le boîtier redémarre ensuite.</p>
      <button class="btn gris" onclick="chercher('r1','lr1')">Chercher les Wi-Fi autour</button>
      <select id="lr1" class="cache" onchange="$('r1').value=this.value"></select>
      <label for="r1">Nom du nouveau Wi-Fi</label><input id="r1" type="text" autocapitalize="none" autocomplete="off">
      <label for="r2">Mot de passe de ce Wi-Fi</label><input id="r2" type="password">
      <label for="r0a">Ton code secret actuel <small>(pour confirmer)</small></label><input id="r0a" type="password">
      <button class="btn" onclick="regler('wifi')">Enregistrer le Wi-Fi</button>
    </details>
    <details>
      <summary>Changer le code secret</summary>
      <p>Tous les téléphones devront retaper le nouveau code. 6 caractères minimum.</p>
      <label for="r0b">Code secret actuel</label><input id="r0b" type="password">
      <label for="r3">Nouveau code secret</label><input id="r3" type="password">
      <label for="r4">Retape le nouveau code</label><input id="r4" type="password">
      <button class="btn" onclick="regler('code')">Changer le code secret</button>
    </details>
    <details>
      <summary>Téléphones mémorisés</summary>
      <p>Si tu perds un téléphone, ce bouton leur fait oublier le code à tous. Chacun devra le retaper.</p>
      <button class="btn rouge" onclick="oublier()">Oublier tous les téléphones</button>
    </details>
  </div>

  <div class="panel pad gap">
    <h2>Voyant bleu du boîtier</h2>
    <div class="led">Fixe : connecté au Wi-Fi de la maison.<br>Clignotant : mode configuration ou connexion en cours.<br>Éteint : aucun Wi-Fi connecté.</div>
  </div>

  <div class="panel pad gap">
    <h2>Bouton BOOT : mode configuration</h2>
    <p style="margin:0">Si le Wi-Fi ne marche plus (changement de box par exemple), maintiens le bouton <strong>BOOT</strong> du boîtier pendant <strong>3 secondes</strong>. Le voyant bleu clignote : connecte ton téléphone au Wi-Fi <strong>ESP-Config</strong>, puis ouvre <strong>192.168.4.1</strong>. Aucun réglage n'est effacé.</p>
  </div>
</section>

</main>
<nav><div>
  <button id="n-porte" class="active" onclick="page('porte')"><span>⌂</span>Porte</button>
  <button id="n-histo" onclick="page('histo')"><span>◷</span>Historique</button>
  <button id="n-regl" onclick="page('regl')"><span>⚙</span>Réglages</button>
</div></nav>
</div>

<div id="voile" class="cache" onclick="if(event.target==this)fermer()">
  <div id="feuille"><h3 id="vt"></h3><p id="vp"></p><button class="btn" id="vok">Confirmer</button><button class="btn gris" onclick="fermer()">Annuler</button></div>
</div>
<div id="toast"></div>



<script>
var $=function(i){return document.getElementById(i);};
var NL=String.fromCharCode(10);
function lireTheme(){try{return localStorage.getItem('theme-acces');}catch(e){return null;}}
var tt,derniere='',animee='',sim=false,pageCourante='porte',temps=lireTheme()||'auto';
function toast(t,c){var e=$('toast');e.textContent=t;e.className='vu '+(c||'');clearTimeout(tt);tt=setTimeout(function(){e.className='';},3600);}
async function api(url,corps){
  var o=(corps===undefined)?{}:{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:corps};
  var r=await fetch(url,o);return {ok:r.ok,txt:await r.text()};
}
function lignes(t){return t.split(NL).filter(function(l){return l;});}
function decouper(l){var i=l.indexOf('  ');return i<0?{h:'',t:l}:{h:l.slice(0,i),t:l.slice(i+2)};}
function enc(v){return encodeURIComponent(v);}
function dirDe(t){return t.indexOf('Ouverture (')===0?'ouverture':t.indexOf('Fermeture (')===0?'fermeture':'';}
function detail(t){var m=/\(([^)]*)\)/.exec(t);return m?m[1]:'';}

/* apparence */
function appliquerTheme(){
  var sombre=temps==='sombre'||(temps==='auto'&&matchMedia('(prefers-color-scheme: dark)').matches);
  document.body.classList.toggle('dark',sombre);
  ['clair','sombre','auto'].forEach(function(x){$('th-'+x).classList.toggle('sel',temps===x);});
}
function theme(x){temps=x;try{localStorage.setItem('theme-acces',x);}catch(e){}appliquerTheme();}
try{matchMedia('(prefers-color-scheme: dark)').addEventListener('change',appliquerTheme);}catch(e){}

/* écrans et pages */
function ecran(n){
  $('ec-setup').className='centre'+(n==='setup'?'':' cache');
  $('ec-login').className='centre'+(n==='login'?'':' cache');
  $('ec-app').className=(n==='app')?'':'cache';
}
function page(id){
  pageCourante=id||'porte';
  ['porte','histo','regl'].forEach(function(x){
    $(x).className=(x===pageCourante)?'':'cache';
    $('n-'+x).className=(x===pageCourante)?'active':'';
  });
  if(pageCourante==='histo')actualiser();
  if(pageCourante==='regl'){chargerReglages();chargerLumiere(true);}
  window.scrollTo(0,0);
}

/* démarrage et connexion */
async function demarrer(){
  appliquerTheme();
  try{var r=await api('/etat');
    if(r.txt==='setup')ecran('setup');
    else if(r.ok){ecran('app');page('porte');actualiser();}
    else ecran('login');
  }catch(e){ecran('login');toast('Pas de réponse du boîtier.','err');}
}
async function entrer(){
  if(!$('nom').value.trim()){toast('Tape ton prénom.','err');return;}
  try{var r=await api('/connexion','mdp='+enc($('p').value)+'&nom='+enc($('nom').value));
    if(r.ok){$('p').value='';ecran('app');page('porte');actualiser();}else toast(r.txt,'err');
  }catch(e){toast('Pas de réponse du boîtier.','err');}
}

/* synchronisation avec le boîtier (toutes les 2,5 s) */
function animerPorte(dir){
  if(dir===animee)return;animee=dir;
  $('lames').classList.toggle('open',dir==='ouverture');
}
function majBoutons(){
  $('b-ouvrir').classList.toggle('grise',derniere==='ouverture');
  $('b-fermer').classList.toggle('grise',derniere==='fermeture');
  $('forcer').classList.toggle('cache',!derniere);
}
function majSim(actif,minutes){
  sim=actif;
  document.body.classList.toggle('sim',actif);
  $('sim').classList.toggle('cache',!actif);
  $('sim').textContent='MODE SIMULATION : la porte ne bouge pas'+(actif&&minutes?' (encore '+minutes+' min)':'');
  $('simcase').checked=actif;
  $('pill').textContent=actif?'● Simulation':'● Prête';
  $('pill').classList.toggle('sim',actif);
}
async function actualiser(){
  try{
    var r=await api('/historique');if(!r.ok){ecran('login');return;}
    var L=lignes(r.txt).map(decouper);
    var e=L.filter(function(x){return dirDe(x.t);})[0];
    derniere=e?dirDe(e.t):'';animerPorte(derniere);majBoutons();
    if(e){var d=detail(e.t);$('d-lib').textContent=derniere==='ouverture'?'Demande d’ouverture':'Demande de fermeture';
      $('d-det').textContent=d+' · '+e.h;$('d-ico').textContent=derniere==='ouverture'?'↑':'↓';$('d-ico').className='ico o';}
    else{$('d-lib').textContent='Aucune activité';$('d-det').textContent='';$('d-ico').textContent='•';$('d-ico').className='ico';}
    var h='';
    L.forEach(function(){h+='<div class="item"><div class="ico"></div><div><strong></strong><small></small></div><time></time></div>';});
    $('liste').innerHTML=h;
    var items=$('liste').children;
    L.forEach(function(x,i){
      var di=dirDe(x.t),err=x.t.indexOf('Mot de passe')===0||x.t.indexOf('Blocage')===0;
      var ic=items[i].querySelector('.ico');
      ic.className='ico '+(di?'o':err?'err':'');
      ic.textContent=di==='ouverture'?'↑':di==='fermeture'?'↓':err?'!':'•';
      items[i].querySelector('strong').textContent=di==='ouverture'?'Demande d’ouverture':di==='fermeture'?'Demande de fermeture':x.t;
      items[i].querySelector('small').textContent=di?detail(x.t):'';
      items[i].querySelector('time').textContent=x.h;});
    var s=await api('/simulation');
    var p=s.txt.split(' ');majSim(p[0]==='1',parseInt(p[1],10)||0);
  }catch(e){}
}
setInterval(function(){if(!document.hidden&&!$('ec-app').classList.contains('cache'))actualiser();},2500);
document.addEventListener('visibilitychange',function(){if(!document.hidden&&!$('ec-app').classList.contains('cache'))actualiser();});

/* commandes */
function feuille(titre,texte,bouton,action,danger){
  $('vt').textContent=titre;$('vp').textContent=texte;$('vok').textContent=bouton;$('vok').className='btn'+(danger?' rouge':'');
  $('voile').className='';$('vok').onclick=function(){fermer();action();};
}
function fermer(){$('voile').className='cache';}
function demander(type){
  if(type===derniere){
    toast((type==='ouverture'?'Une ouverture':'Une fermeture')+' a déjà été demandée. Utilise « Forcer » si tu es sûr.');return;}
  feuille(type==='ouverture'?'Ouvrir la porte ?':'Fermer la porte ?',
    sim?'SIMULATION : la porte ne bougera pas.':'La porte va '+(type==='ouverture'?'s’ouvrir.':'se fermer.'),
    type==='ouverture'?'Ouvrir':'Fermer',function(){ordre(type);});
}
function forcer(){
  var t=derniere;if(!t)return;
  feuille('Forcer la commande ?',(t==='ouverture'?'Une ouverture':'Une fermeture')+' a déjà été demandée. La position réelle est inconnue.'+(sim?' (Simulation)':''),
    'Forcer',function(){ordre(t);});
}
async function ordre(type){
  $('pill').textContent='● Envoi…';
  try{var r=await api('/impulsion','sens='+enc(type));
    if(r.txt.indexOf('Connecte-toi')>=0){ecran('login');return;}
    toast(r.txt,r.ok?'ok':'err');
    if(r.ok){animee='';await actualiser();}
  }catch(e){toast('Pas de réponse du boîtier.','err');}
  $('pill').textContent=sim?'● Simulation':'● Prête';
}

/* réglages */
async function chargerReglages(){
  try{var r=await api('/wifi');$('wifi-actuel').textContent=r.ok?r.txt:'indisponible';}catch(e){$('wifi-actuel').textContent='indisponible';}
}
async function basculerSim(on){
  try{await api('/simulation','actif='+(on?'1':'0'));await actualiser();
    toast(on?'Simulation activée : le relais ne cliquera pas.':'Simulation arrêtée : les ordres sont réels.',on?'':'ok');}
  catch(e){toast('Pas de réponse du boîtier.','err');$('simcase').checked=sim;}
}
async function chargerLumiere(init){
  try{var r=await api('/lumiere');if(!r.ok)return;
    var p=r.txt.split(' ');
    $('lum-v').textContent=p[0];$('lum-r').textContent=p[1];$('lum-e').textContent=p[2];
    $('lum-a').textContent=p[6]==='1'?'ampoule allumée':'ampoule éteinte';
    if(init){$('lum-seuil').value=p[3];$('lum-fen').value=p[4];$('lum-actif').checked=p[5]==='1';}
  }catch(e){}
}
async function enregistrerLumiere(){
  try{var r=await api('/lumiere','seuil='+enc($('lum-seuil').value)+'&fenetre='+enc($('lum-fen').value)+'&actif='+($('lum-actif').checked?'1':'0'));
    toast(r.ok?'Détecteur enregistré.':r.txt,r.ok?'ok':'err');if(r.ok)chargerLumiere(true);}
  catch(e){toast('Pas de réponse du boîtier.','err');}
}
setInterval(function(){if(!document.hidden&&pageCourante==='regl'&&!$('ec-app').classList.contains('cache'))chargerLumiere(false);},1000);
async function chercher(champ,liste){
  toast('Recherche en cours…');
  try{var r=await api('/reseaux');if(!r.ok){toast(r.txt,'err');return;}
    var s=$(liste);s.innerHTML='<option value="">Choisis ton Wi-Fi dans la liste</option>';
    lignes(r.txt).forEach(function(n){var o=document.createElement('option');o.value=n;o.textContent=n;s.appendChild(o);});
    s.className='';toast('Liste prête : choisis ton Wi-Fi.','ok');
  }catch(e){toast('Pas de réponse du boîtier.','err');}
}
async function envoyerConfig(corps,msg){
  try{var r=await api('/config',corps);toast(r.ok?msg:r.txt,r.ok?'ok':'err');return r.ok;}
  catch(e){toast('Pas de réponse du boîtier.','err');return false;}
}
async function installer(){
  if(!$('s1').value){toast('Étape 1 : choisis ou tape le nom du Wi-Fi.','err');return;}
  if($('s3').value.length<6){toast('Étape 2 : le code secret doit avoir 6 caractères mini.','err');return;}
  if($('s3').value!==$('s4').value){toast('Les deux codes secrets ne sont pas identiques.','err');return;}
  await envoyerConfig('actuel=&ssid='+enc($('s1').value)+'&wp='+enc($('s2').value)+'&mdp='+enc($('s3').value),'Enregistré. Le boîtier redémarre : reconnecte ton téléphone à ton Wi-Fi, puis ouvre esp-acces.local');
}
async function regler(quoi){
  if(quoi==='wifi'){
    if(!$('r1').value){toast('Choisis ou tape le nom du nouveau Wi-Fi.','err');return;}
    await envoyerConfig('actuel='+enc($('r0a').value)+'&ssid='+enc($('r1').value)+'&wp='+enc($('r2').value)+'&mdp=','Wi-Fi enregistré. Le boîtier redémarre (10 secondes).');
  }else{
    if($('r3').value.length<6){toast('Le nouveau code doit avoir 6 caractères mini.','err');return;}
    if($('r3').value!==$('r4').value){toast('Les deux nouveaux codes ne sont pas identiques.','err');return;}
    await envoyerConfig('actuel='+enc($('r0b').value)+'&ssid=&wp=&mdp='+enc($('r3').value),'Code changé. Le boîtier redémarre : retape le nouveau code.');
  }
}
function oublier(){
  feuille('Oublier tous les téléphones ?','Chacun devra retaper le code secret.','Oui, oublier',async function(){
    try{var r=await api('/oublier','');toast(r.txt,r.ok?'ok':'err');if(r.ok)ecran('login');}catch(e){toast('Pas de réponse du boîtier.','err');}},true);
}
function effacer(){
  feuille('Effacer l’historique ?','Tous les événements seront supprimés. Cette action est définitive : pense à exporter avant.','Oui, effacer',async function(){
    try{var r=await api('/effacer-historique','');toast(r.txt,r.ok?'ok':'err');if(r.ok)actualiser();}catch(e){toast('Pas de réponse du boîtier.','err');}},true);
}
async function exporter(){
  try{var r=await api('/historique');if(!r.ok){ecran('login');return;}
    var csv=String.fromCharCode(0xFEFF)+'Date;Événement'+NL+lignes(r.txt).map(function(x){var e=decouper(x);return e.h+';'+e.t;}).reverse().join(NL);
    var a=document.createElement('a');a.href=URL.createObjectURL(new Blob([csv],{type:'text/csv;charset=utf-8'}));
    a.download='historique.csv';document.body.appendChild(a);a.click();a.remove();toast('Fichier historique.csv téléchargé.','ok');
  }catch(e){toast('Export impossible.','err');}
}
try{if('serviceWorker' in navigator)navigator.serviceWorker.register('/sw.js');}catch(e){}
demarrer();
</script>
</body></html>)rawliteral";
