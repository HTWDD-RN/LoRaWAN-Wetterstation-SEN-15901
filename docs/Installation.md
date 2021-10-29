# Einrichtung der Arduino IDE

## Ubuntu 21.04

1. Arduino IDE herunterladen (https://www.arduino.cc/en/donate/)

2. Installationsanleitung folgen: https://ubuntu.com/tutorials/install-the-arduino-ide#2-installing-via-a-tarball

3. Arduino anschließen

4. Arduino IDE starten mit `sudo arduino`

5. Unter Werkzeuge->Board "Arduino Uno" wählen und unter Werkzeuge->Port "/dev/ttyUSB0"

6. Um die Funktionalität zu testen, ein Testskript ausführen: Datei->Beispiele->01 Basics->Blink, dann "Hochladen" drücken. Die "L" LED (PIN 13) sollte aufblinken.

## VSCode Extenstion

1. Installation von `vsciot-vscode.vscode-arduino` Microsoft Extension für VSCode in der Version 0.4.5

2. In den VSCode Einstellungen folgende Zeilen hinzufügen (Pfad zum arduino binary Verzeichnis ggf. ändern):

```
"arduino.path": "/usr/local/bin/arduino/arduino-1.8.16",
"arduino.logLevel": "info",
"arduino.enableUSBDetection": true,
"C_Cpp.intelliSenseEngine": "Tag Parser"
```

3. Im Terminal Berechtigung für den USB Port `/dev/ttyUSB0` aktivieren:

```
sudo usermod -a -G /dev/ttyUSB0 $USER
```

und ggf.:

```
sudo chmod a+rw /dev/ttyUSB0
```

4. VSCode neustarten

5. eine `test.ino` Datei erstellen, um Arduino Optionen einsehen zu können

6. Arduino anschließen

7. Board auf `Arduino Uno` einstellen, Programmer auf `avrispmkii` einstellen, Port auf `/dev/ttyUSB0 einstellen`