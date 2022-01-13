# Environment
## Arduino IDE

### Ubuntu 21.04

1. Arduino IDE herunterladen (https://www.arduino.cc/en/donate/)

2. Installationsanleitung folgen: https://ubuntu.com/tutorials/install-the-arduino-ide#2-installing-via-a-tarball

3. Arduino anschließen

4. Arduino IDE starten mit `sudo arduino`

5. Unter Werkzeuge->Board "Arduino Uno" wählen und unter Werkzeuge->Port "/dev/ttyUSB0"

6. Um die Funktionalität zu testen, ein Testskript ausführen: Datei->Beispiele->01 Basics->Blink, dann "Hochladen" drücken. Die "L" LED (PIN 13) sollte aufblinken.

## Bibliotheken einbinden
1. Bibliothek `Arduino-LMIC` als ZIP herunterladen (https://github.com/dragino/arduino-lmic)

2. In der Arduino-IDE: Sketch -> Bibliothek einbinden -> .ZIP-Bibliothek hinzufügen

## Dragino konfigurieren <!-- TODO: mehr dokumentieren -->
1. LoRaWAN
    - Service Provider - The Things Network V3
2. LoRa
    - Frequency: 868100000
    - Spreading Factor: SF7

<!-- TODO: Gateway ID des Dragino muss als Gateway EUI im TTN eingetragen werden. Gateway ID im TTN beliebig-->

## TTN
1. Add application
2. Add end device
2.1 Manually
2.1.1 Frequency Plan: Europe 863-870 MHz (SF9 for RX2 - recommended)
2.1.2 LoRaWan version: MAC V1.0.4
2.1.3 Regional Parameters version: PHY V1.1 REV A
2.1.4 Show advanced activation !
2.1.5 Activation mode: Activation by personalization (ABP)
2.1.6 Device address, AppSKey, NwkSKey generieren und in Code einfügen
2.1.7 Register end device


## VSCode Extenstion

1. Installation von `vsciot-vscode.vscode-arduino` Microsoft Extension

2. In den VSCode Einstellungen folgende Zeilen hinzufügen (Pfad zum arduino binary Verzeichnis ggf. ändern):

```json
"arduino.path": "/usr/local/bin/arduino/arduino-1.8.16",
"arduino.logLevel": "info",
"arduino.enableUSBDetection": true,
"C_Cpp.intelliSenseEngine": "Tag Parser"
```

3. Im Terminal Berechtigung für den USB Port `/dev/ttyUSB0` aktivieren:

```
sudo chmod a+rw /dev/ttyUSB0
```

4. VSCode neustarten

5. eine `test.ino` Datei erstellen, um Arduino Optionen einsehen zu können

6. Arduino anschließen

7. Board auf `Arduino Uno` einstellen, Programmer auf `avrispmkii` einstellen, Port auf `/dev/ttyUSB0 einstellen`


### LaTeX in VSCode

1. LaTeX-Pakete installieren:

```
sudo apt install texlive texlive-lang-german texlive-latex-extra latexmk
```

2. VSCode Extension `james-yu.latex-workshop` installieren

3. Gegebenenfalls VSCode neustarten