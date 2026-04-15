# OT_RANGE_V2 - Rapport de preparation au deploiement

## Objectif

`OT_RANGE_V2` est une variante de deploiement allegee de la cyber-range OT, preparee pour le scenario suivant :

- une VM `L1/L2/L3` qui heberge le lab OT
- `R1` et `R2` avec port mirroring logiciel
- un conteneur Debian unique en `L3` qui termine les deux miroirs reseau
- un futur deploiement `Malcolm hedgehog` sur cette VM
- un `Malcolm principal` complet sur une autre VM

Cette variante ne deploie pas encore Malcolm ni Wazuh.

## Ce qui a ete ajoute

- `docker-compose.yml`
  - topologie `L1`, `L2`, `L3`
  - services OT essentiels
  - `EWS`
  - `Kali Attacker`
  - `Portal`
  - `R1`, `R2`
  - un capteur Debian unique de mirroring en `L3`
  - viewer 3D minimal de la station B
- `router1/` et `router2/`
  - versions propres des routeurs
  - Suricata local conserve
  - aucun agent Wazuh embarque
  - port mirroring `tc + gretap`
- `sensor_l3/`
  - Debian minimal
  - reception des copies de trafic via `gretap`
  - interfaces de capture `mirror_l1` et `mirror_l2`
  - bridge de capture `br-mirror`
  - repertoires prepares pour futurs logs Suricata, Zeek et PCAP
- `viewer3d-station-b/`
  - version minimale
  - backend Python
  - frontend statique
  - uniquement les assets necessaires au rendu 3D
- `portal/`
  - service Node.js minimal
  - portail d'authentification et de gestion d'utilisateurs
  - persistance locale dans `portal/data/users.json`
- `station_a/plc_a/utils` et `station_b/plc_b/utils`
  - dependances OpenPLC ajoutees au dossier de deploiement
  - necessaires a une future reconstruction propre des images PLC dans la VM cible

## Topologie validee

- `L1` : `PLC-A 192.168.10.10`, `SCADA-A 192.168.10.20`, `R1 192.168.10.254`
- `L2` : `PLC-B 192.168.20.10`, `SCADA-B 192.168.20.20`, `Viewer3D 192.168.20.90`, `R2 192.168.20.254`
- `L3` : `SCADA Central 192.168.30.10`, `EWS 192.168.30.20`, `Kali 192.168.30.30`, `Portal 192.168.30.60`, `R1 192.168.30.254`, `R2 192.168.30.253`
- capteur unique :
  - `ot_sensor 192.168.30.50`

## Role du Portal

Le `portal` est un petit service web Node.js qui fournit :

- une authentification locale
- une gestion des utilisateurs
- une gestion simple des roles (`admin`, `superadmin`, `user`)
- une persistence des comptes dans `portal/data/users.json`

Il ne remplace pas les SCADA ni les postes OT.
Son role est de fournir un point d'acces applicatif leger cote `L3`.

## Port mirroring mis en place

### R1

- interface tunnel : `gretap_l1`
- destination miroir : `192.168.30.50`
- cle tunnel : `101`
- actions `tc` sur `eth0` et `eth1`

### R2

- interface tunnel : `gretap_l2`
- destination miroir : `192.168.30.50`
- cle tunnel : `102`
- actions `tc` sur `eth0` et `eth1`

### Capteur Debian L3

- `ot_sensor` cree :
  - `mirror_l1`
  - `mirror_l2`
  - `br-mirror`
- `mirror_l1` et `mirror_l2` sont rattachees au bridge `br-mirror`
- les trois interfaces sont `UP`

## Corrections fonctionnelles integrees

### OpenPLC

Probleme trouve :
- les PLC demarraient l'interface web mais pas le runtime Modbus
- cause : `Start_run_mode = false`

Correction appliquee dans `OT_RANGE_V2` :
- `station_a/plc_a/entrypoint.sh`
- `station_b/plc_b/entrypoint.sh`

Les deux scripts forcent maintenant :
- `Start_run_mode = true` dans `/docker_persistent/openplc.db`

Resultat :
- `PLC-A` ecoute bien sur `502`
- `PLC-B` ecoute bien sur `502`

Note de validation locale :
- avant nettoyage final du dossier, une validation locale acceleree a ete faite sur des images deja presentes
- sur ces images existantes, le runtime a ete demarre via l'API OpenPLC (`/api/start-plc`) apres creation du compte `admin`
- pour la VM cible, l'objectif est bien de reconstruire les images PLC depuis `OT_RANGE_V2`

### Viewer 3D

Probleme trouve :
- le backend du viewer ratait sa connexion initiale au PLC
- `/api/status` ne tentait pas de reconnexion

Correction appliquee :
- `viewer3d-station-b/backend/telemetry_server.py`

Resultat :
- le viewer revoit maintenant `PLC-B`
- statut `live`
- source `openplc-modbus`

## Validation locale effectuee

### Services demarres

Etat valide via `docker compose ps` :

- `plc_station_a`
- `plc_station_b`
- `scada_station_a`
- `scada_station_b`
- `scada_scentral`
- `router_r1_r3`
- `router_r2_r3`
- `ot_sensor`
- `viewer3d_station_b`

### Validation reseau et mirroring

Valide :

- `gretap_l1` present sur `R1`
- `gretap_l2` present sur `R2`
- filtres `tc mirred` presents sur `eth0` et `eth1` des deux routeurs
- `mirror_l1` present sur le capteur `ot_sensor`
- `mirror_l2` present sur le capteur `ot_sensor`
- `br-mirror` present sur le capteur `ot_sensor`

### Validation trafic OT

Valide :

- `SCADA Central -> PLC-A:502` fonctionne
- `SCADA Central -> PLC-B:502` fonctionne
- `tcpdump` sur `mirror_l1` voit le trafic Modbus vers `192.168.10.10:502`
- `tcpdump` sur `mirror_l2` voit le trafic Modbus vers `192.168.20.10:502`
- `tcpdump` sur `br-mirror` voit le trafic agrege des deux zones
- les captures montrent a la fois les flux `L1` et `L2` sur la meme interface agregee `br-mirror`

### Validation viewer 3D

Valide :

- le service est `healthy`
- le backend repond
- `api/status` retourne :
  - `plc_connected: true`
  - `mode: live`
  - `source: openplc-modbus`

## Commandes utiles

### Deploiement propre

```bash
cd /home/kakashi_/ICSHUB/OT_RANGE_V2
docker compose up -d --build
```

### Verification rapide du mirroring

```bash
docker compose exec router_r1_r3 sh -lc 'ip link show gretap_l1; tc filter show dev eth0 ingress; tc filter show dev eth1 ingress'
docker compose exec router_r2_r3 sh -lc 'ip link show gretap_l2; tc filter show dev eth0 ingress; tc filter show dev eth1 ingress'
docker compose exec ot_sensor sh -lc 'ip link show mirror_l1'
docker compose exec ot_sensor sh -lc 'ip link show mirror_l2'
docker compose exec ot_sensor sh -lc 'ip link show br-mirror'
```

### Verification trafic

```bash
docker compose exec ot_sensor sh -lc 'tcpdump -ni mirror_l1 tcp port 502'
docker compose exec ot_sensor sh -lc 'tcpdump -ni mirror_l2 tcp port 502'
docker compose exec ot_sensor sh -lc 'tcpdump -ni br-mirror tcp port 502'
```

### Verification viewer 3D

```bash
docker compose exec viewer3d_station_b python -c "import urllib.request; print(urllib.request.urlopen('http://localhost:8090/api/status').read().decode())"
```

## Couplage futur avec Hedgehog

Le design actuel est volontairement prepare pour un futur `Malcolm hedgehog` unique.

Principe :

- `ot_sensor` porte deja le reseau de capture
- il expose une interface de capture agregee `br-mirror`
- un futur Hedgehog pourra :
  - soit etre installe directement dans la VM sur cette interface
  - soit etre deploie en conteneurs partageant le namespace reseau de `ot_sensor`

Dans ce deuxieme cas, le pattern sera le meme que celui deja valide dans l'ancien labo :

- `ot_sensor` garde les interfaces reseau
- les conteneurs `suricata-live`, `zeek-live`, `arkime-live` utilisent son namespace reseau
- l'interface a selectionner dans l'installation Hedgehog sera `br-mirror`

Lecture simple de l'architecture future :

- `R1` copie le trafic `L1` vers `mirror_l1`
- `R2` copie le trafic `L2` vers `mirror_l2`
- `ot_sensor` agrege les deux flux sur `br-mirror`
- Hedgehog Linux ou Malcolm Hedgehog ecoute `br-mirror`
- le Malcolm principal sur une autre VM recoit ensuite les artefacts du capteur

## Wazuh futur

Le dossier prepare aussi le cas ou un agent Wazuh serait ajoute plus tard dans `ot_sensor`.

Les chemins deja montes sont :

- `/sensor-logs/suricata`
- `/sensor-logs/zeek`
- `/sensor-pcap`

Donc, si plus tard :

- `Suricata` du capteur ecrit dans `/sensor-logs/suricata`
- `Zeek` ecrit dans `/sensor-logs/zeek`

alors un agent Wazuh dans `ot_sensor` pourra lire ces fichiers via volume partage, exactement comme pour le capteur precedent.

En pratique, cela veut dire que le Debian `ot_sensor` peut jouer deux roles plus tard :

- terminaison reseau des miroirs
- point de collecte local pour un agent Wazuh

Le futur agent n'aura pas besoin d'ecouter le trafic brut. Il lira simplement les journaux que les sondes du Hedgehog ecriront sur disque.

En l'etat, `OT_RANGE_V2` est pret pour servir de base propre de deploiement, accueillir un Hedgehog unique, et recevoir plus tard un agent Wazuh sans refonte de topologie.
