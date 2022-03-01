# LoRaWAN Wetterstation SEN-15901
## Motivation
>Die analoge Wetterstation SEN-15901 soll mithilfe eines Arduino Uno Daten über LoRa versenden und im The Things Network (TTN) ausgewertet werden.
>
>Die Anwendung entstand im Rahmen des Projektseminars 2021/22 bei Prof. Dr. Jörg Vogt an der HTW-Dresden.

## [Dokumentation](https://github.com/HTWDD-RN/LoRaWAN-Wetterstation-SEN-15901/blob/main/docs/documentation.pdf)

Für eine Erklärung und einen genauen Aufbau betrachten Sie bitte die Dokumentation.


## Übersicht

Das Repository ist folgendermaßen aufgebaut:

```
LoRaWAN-Wetterstation-SEN-15901
├── docs
│   ├── documentation.pdf               Projektdokumentation
│   ├── documentation.tex               Projektdokumentation LaTeX-Quelldatei
│   ├── img                             enthält Bilder für die Dokumentation
│   │   └── *
│   ├── links.md                        Linksammlung (Wetterstation, Arduino)
│   └── schaltplan.fzz                  Fritzing Schaltplan der Anwendung
├── README.md                           
└── src         
    ├── local_weather_station           Implementation ohne LoRa
    │   └── local_weather_station.ino
    └── lora_weather_station            Implementation mit LoRa
        ├── config.h                    Konfigurationsdatei für den Nutzer
        ├── lora_weather_station.ino    Hauptprogramm für Arduino
        ├── payload_formatter.js        Uplink Payload Formatter für TTN
        ├── secret.template.h           Template für Secrets
        ├── ttn_send.cpp                LoRa Sendefunktionalität
        └── ttn_send.h                  
```

## Voraussetzungen
Um die Anwendung zu nutzen, wird folgende Hardware benötigt:
* Wetterstation SEN-15901
* Arduino Uno
* Dragino LoRa Shield 1.4 für Arduino
* LoRa-Gateway (sofern nicht in Reichweite)
* 2x RJ11 Einbaubuchsen
* 1x Widerstand (10 kOhm)
* Breadboard, Jumper-Kabel

Außerdem muss ein TTN-Account angelegt werden.


## Installation 

<details open>

### Arduino IDE

1. Arduino IDE herunterladen (https://www.arduino.cc/en/software)

2. Installationsanleitung folgen: https://ubuntu.com/tutorials/install-the-arduino-ide#2-installing-via-a-tarball

3. Arduino Uno anschließen

4. Arduino IDE starten mit `sudo arduino`

5. Unter `Werkzeuge->Board` "Arduino Uno" wählen 

6. Unter `Werkzeuge->Port` "/dev/ttyUSB0" wählen

7. Um die Funktionalität zu testen, kann ein Testskript ausgeführt werden: 
    * Dazu in der Arduino IDE `Datei->Beispiele->01 Basics->Blink` wählen, dann "Hochladen" drücken.
    * Die "L" LED (PIN 13) des Arduino Uno sollte aufblinken.

### LMIC Bibliothek einbinden (für LoRa Shield 1.4)

1. Bibliothek `Arduino-LMIC` als ZIP herunterladen (https://github.com/dragino/arduino-lmic)

2. Arduino IDE starten

3. Unter `Sketch->Bibliothek einbinden->.ZIP-Bibliothek hinzufügen` die heruntergeladene ZIP hinzufügen

4. Die Frequenz in der LMIC-Library anpassen (siehe https://github.com/dragino/arduino-lmic#configuration)

    * Die benötigte Datei befindet sich unter `home/Arduino/libraries/arduino-lmic-master/src/lmic/config.h`
    * Hier muss die Region gegebenenfalls angepasst werden.

### VSCode Arduino Extension

1. `vsciot-vscode.vscode-arduino` Microsoft Extension installieren

2. In den VSCode Einstellungen folgende Zeilen hinzufügen (Pfad zum Arduino Binary Verzeichnis ggf. ändern):

```json
"arduino.path": "/usr/local/bin/arduino/arduino-1.8.16",
"arduino.logLevel": "info",
"arduino.enableUSBDetection": true,
"C_Cpp.intelliSenseEngine": "Tag Parser"
"arduino.defaultBaudRate": 9600
```

3. Im Terminal die Berechtigung für den USB Port `/dev/ttyUSB0` aktivieren:

```
sudo chmod a+rw /dev/ttyUSB0
```

4. VSCode neustarten

5. eine `lora_weather_station.ino` Datei öffnen, um Arduino Optionen einsehen zu können

6. Arduino Uno anschließen

7. In der Taskleiste folgende Optionen einstellen:

    * das Board auf `Arduino Uno`
    * Programmer auf `avrispmkii`
    * Port auf `/dev/ttyUSB0`

* Der Serielle Monitor ist dort ebenfalls für Debugging verfügbar

### VSCode LaTeX Extension (optional)

1. LaTeX-Pakete installieren:

```
sudo apt install texlive texlive-lang-german texlive-latex-extra latexmk
```

2. VSCode Extension `james-yu.latex-workshop` installieren

3. Gegebenenfalls VSCode neustarten

### TTN

#### OTAA (empfohlen)
1. Add application
2. Add end device
    1. Manually
    2. Frequency Plan: Europe 863-870 MHz (SF9 for RX2 - recommended)
    3. LoRaWan version: MAC V1.0.2
    4. Regional Parameters version: PHY V1.0.2 REV A
    5. DeEUI, AppEUI, AppKey generieren und in Code einfügen (lsb beachten)
    6. Register end device
#### ABP (alternativ)
1. Add application
2. Add end device
    1. Manually
    2. Frequency Plan: Europe 863-870 MHz (SF9 for RX2 - recommended)
    3. LoRaWan version: MAC V1.0.2
    4. Regional Parameters version: PHY V1.0.2 REV A
    5. Show advanced activation !
    6. Activation mode: Activation by personalization (ABP)
    7. Device address, AppSKey, NwkSKey generieren und in Code einfügen
    8. Register end device
    9. Frame Counter deaktivieren
    10. In den General settings Tab wechseln
    11. unter Network Layer -> Expand
    12. unten Advanced MAC settings
    13. das Häkchen in Resets frame counters setzen
#### ADR deaktivieren (optional)
1. In den General settings Tab wechseln
2. unter Network Layer -> Expand
3. unten Advanced MAC settings
4. das Häkchen in Use adaptive data rate (ADR) entfernen

</details>

## Ausführung

1. TTN End Device erstellen (entweder OTAA oder ABP)
    * siehe Dokumentation
2. Repository in VSCode öffnen
3. Die Datei `secret.template.h` mit den entsprechenden Secrets füllen
4. Die Datei `secret.template.h` umbenennen in `secret.h`
5. Die Konfigurationsdatei `config.h` anpassen
    * Beispiel siehe `config.h`
6. Arduino Uno anschließen
7. Berechtigung für den USB Port aktivieren:
    * `sudo chmod a+rw /dev/ttyUSB0`
8. Die Datei `lora_weather_station.ino` öffnen
9. Die Anwendung auf den Arduino Uno schreiben
10. Zum TTN End Device wechseln und auf Pakete warten

## TODO
* Bilder in Aktion hinzufügen
* Dokumentation auf Englisch übersetzen
* Prüfen warum bei ABP nicht alle Frequenzen funktionieren