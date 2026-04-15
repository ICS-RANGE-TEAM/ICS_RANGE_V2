#!/bin/bash
set -e

# PLC Station B — L2 (192.168.20.10), gateway R2-R3 = 192.168.20.254
echo "Configuration des routes PLC Station B..."
ip route add 192.168.30.0/24 via 192.168.20.254 || true
ip route add 192.168.10.0/24 via 192.168.20.254 || true

mkdir -p /docker_persistent/st_files
cp -n /workdir/webserver/openplc_default.db /docker_persistent/openplc.db
cp -n /workdir/webserver/dnp3_default.cfg /docker_persistent/dnp3.cfg
cp -n /workdir/webserver/active_program_default /docker_persistent/active_program
cp -n /dev/null /docker_persistent/persistent.file
cp -n /dev/null /docker_persistent/mbconfig.cfg
cp -n /workdir/webserver/st_files_default/* /docker_persistent/st_files/ 2>/dev/null || true

python3 - <<'PY'
import sqlite3
from pathlib import Path

db = Path("/docker_persistent/openplc.db")
program_file = "stationB.st"

conn = sqlite3.connect(db)
cur = conn.cursor()
cur.execute(
    """CREATE TABLE IF NOT EXISTS Programs (
        Prog_ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
        Name TEXT NOT NULL,
        Description TEXT,
        File TEXT NOT NULL,
        Date_upload INTEGER NOT NULL
    )"""
)
cur.execute(
    """CREATE TABLE IF NOT EXISTS Settings (
        Key TEXT NOT NULL UNIQUE,
        Value TEXT NOT NULL,
        PRIMARY KEY(Key)
    )"""
)
default_programs = [
    ("Blank Program", "Dummy empty program", "blank_program.st"),
    ("dispatch.st", "Programme de dispatch electric", "dispatch.st"),
    ("webserver_program", "Programme reserve a l'interface web OpenPLC", "webserver_program.st"),
    ("stationB.st", "Programme OT par defaut de la station B", program_file),
]
for name, description, filename in default_programs:
    cur.execute("SELECT 1 FROM Programs WHERE File = ?", (filename,))
    if cur.fetchone() is None:
        cur.execute(
            "INSERT INTO Programs (Name, Description, File, Date_upload) VALUES (?, ?, ?, strftime('%s','now'))",
            (name, description, filename),
        )

default_settings = {
    "Modbus_port": "502",
    "Dnp3_port": "disabled",
    "Start_run_mode": "true",
    "Slave_polling": "100",
    "Slave_timeout": "1000",
    "Enip_port": "44818",
    "Pstorage_polling": "disabled",
    "snap7": "true",
}
for key, value in default_settings.items():
    cur.execute(
        "INSERT INTO Settings (Key, Value) VALUES (?, ?) "
        "ON CONFLICT(Key) DO UPDATE SET Value=excluded.Value",
        (key, value),
    )
conn.commit()
conn.close()

Path("/docker_persistent/active_program").write_text(program_file + "\n")
PY

echo "Démarrage OpenPLC..."
exec ./start_openplc.sh
