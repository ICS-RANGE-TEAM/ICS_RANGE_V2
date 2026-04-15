# OT_RANGE_V2

Version de deploiement allegee de la cyber-range OT.

## Vue rapide

Cette variante deploie :
- `L1`, `L2`, `L3`
- `PLC-A`, `PLC-B`
- `SCADA-A`, `SCADA-B`, `SCADA Central`
- `R1` et `R2` avec Suricata local et port mirroring logiciel `tc + gretap`
- un capteur Debian unique `ot_sensor` en `L3`
- `EWS`
- `Kali Attacker`
- `Portal` local de gestion d'utilisateurs
- le viewer 3D minimal de la station B

Non inclus dans cette variante :
- Malcolm principal
- Malcolm hedgehog profile
- Wazuh

## Deploy

Commande unique :

```bash
docker compose up -d --build
```

## Services

| Service | Zone | IP | Port hote |
|---|---|---|---|
| `plc_station_a` | L1 | `192.168.10.10` | `8080 -> 8080` |
| `scada_station_a` | L1 | `192.168.10.20` | `1881`, `48010` |
| `plc_station_b` | L2 | `192.168.20.10` | `8081 -> 8080` |
| `scada_station_b` | L2 | `192.168.20.20` | `1882` |
| `viewer3d_station_b` | L2 | `192.168.20.90` | `8090` |
| `scada_scentral` | L3 | `192.168.30.10` | `1884`, `48011` |
| `ews` | L3 | `192.168.30.20` | `6080`, `2222` |
| `kali_attacker` | L3 | `192.168.30.30` | `6081`, `2221`, `5000` |
| `ot_sensor` | L3 | `192.168.30.50` | aucun |
| `portal` | L3 | `192.168.30.60` | `3000` |
| `router_r1_r3` | L1/L3 | `192.168.10.254`, `192.168.30.254` | `1444` |
| `router_r2_r3` | L2/L3 | `192.168.20.254`, `192.168.30.253` | `1443` |

## Notes

- `ot_sensor` termine deja les deux miroirs et expose `mirror_l1`, `mirror_l2` et `br-mirror`
- `br-mirror` est prevu pour un futur Malcolm `hedgehog` unique
- les dependances `utils/` d'OpenPLC ont ete embarquees pour preparer un build propre sur la VM cible
- `portal` est un service Node.js avec authentification locale et persistance dans `portal/data/users.json`

## Verification rapide

```bash
docker compose exec router_r1_r3 tc filter show dev eth0 ingress
docker compose exec router_r2_r3 tc filter show dev eth0 ingress
docker compose exec ot_sensor ip link show mirror_l1
docker compose exec ot_sensor ip link show mirror_l2
docker compose exec ot_sensor ip link show br-mirror
```
