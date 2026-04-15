# Couplage futur entre `ot_sensor`, Hedgehog et Wazuh

## Objectif

Le dossier `OT_RANGE_V2` prepare une architecture de deploiement dans laquelle :

- `R1` et `R2` copient le trafic OT par port mirroring logiciel
- un Debian unique `ot_sensor` en `L3` termine les deux miroirs
- un futur `Malcolm Hedgehog` viendra ecouter ce trafic
- un `Malcolm principal` tournera sur une autre VM

## Role exact du Debian `ot_sensor`

`ot_sensor` n'est pas le Hedgehog lui-meme.

Son role est de servir de **point de terminaison reseau** pour les copies de trafic venant des routeurs :

- `R1 -> mirror_l1`
- `R2 -> mirror_l2`

Puis `ot_sensor` agrege ces deux flux dans une interface logique unique :

- `br-mirror`

Ce design simplifie l'etape suivante :

- au lieu de demander au futur Hedgehog de gerer deux interfaces separees,
- on lui presente une seule interface de capture : `br-mirror`

## Comment Hedgehog Linux sera couple avec ce Debian

Deux approches sont possibles.

### Option 1 - Hedgehog Linux installe directement sur la VM cible

Dans ce cas :

- la VM heberge `L1`, `L2`, `L3`
- `ot_sensor` fournit deja la logique de terminaison des miroirs
- Hedgehog Linux ou un equivalent Malcolm Hedgehog s'installe sur cette meme VM
- l'interface de capture a selectionner est `br-mirror`

Lecture simple :

- `R1` copie le trafic de `L1`
- `R2` copie le trafic de `L2`
- `ot_sensor` le recupere
- `br-mirror` devient l'interface d'ecoute du Hedgehog

### Option 2 - Hedgehog conteneurise plus tard

Dans ce cas :

- `ot_sensor` garde les interfaces reseau `mirror_l1`, `mirror_l2`, `br-mirror`
- les conteneurs Hedgehog `suricata-live`, `zeek-live`, `arkime-live` partagent son namespace reseau
- les sondes ecoutent toutes `br-mirror`

Cette approche est proche de ce qui a deja ete valide dans le lab precedent :

- un conteneur Debian porte la connectivite reseau
- les outils de capture ecoutent dans ce meme namespace

## Pourquoi ce design est propre

- un seul point de capture au lieu de deux capteurs complets
- une seule interface logique pour Hedgehog
- moins de consommation RAM et CPU
- plus simple a maintenir qu'un double capteur
- meilleure preparation pour une VM de deploiement reelle

## Flux futur prevu

```text
R1 -- gretap --> mirror_l1 \
                           \
                            -> br-mirror -> Hedgehog -> Malcolm principal
                           /
R2 -- gretap --> mirror_l2 /
```

## Wazuh plus tard : l'agent pourra-t-il lire les logs du capteur ?

Oui.

Le Debian `ot_sensor` a deja ete prepare pour cela.

Repertoires presents :

- `/sensor-logs/suricata`
- `/sensor-logs/zeek`
- `/sensor-pcap`

Si plus tard un Hedgehog ecrit :

- les alertes Suricata dans `/sensor-logs/suricata`
- les logs Zeek dans `/sensor-logs/zeek`

alors un futur agent Wazuh installe **dans le Debian `ot_sensor`** pourra lire ces fichiers directement.

## Ce que cela veut dire en pratique

Le Debian `ot_sensor` pourra jouer deux roles distincts :

- **role reseau** : terminer les miroirs `R1` et `R2`
- **role collecte SOC** : heberger un agent Wazuh qui lit les logs produits par les sondes

L'agent Wazuh n'aura pas besoin d'inspecter les paquets bruts.
Il lira simplement les fichiers de logs deja produits par :

- Suricata
- Zeek
- eventuellement d'autres composants du capteur

## Conclusion

Le design retenu est le suivant :

- un Debian unique `ot_sensor` en `L3`
- deux tunnels de mirroring distincts
- une interface de capture agregee `br-mirror`
- un futur Hedgehog unique qui ecoutera `br-mirror`
- un futur agent Wazuh qui pourra lire les logs des sondes depuis le Debian

C'est le compromis le plus propre entre :

- simplicite de deploiement
- sobriete en ressources
- lisibilite de l'architecture
- preparation du futur SOC
