# Einrichtung der Arduino IDE

## Ubuntu 21.04

1. Arduino IDE herunterladen (https://www.arduino.cc/en/donate/)

2. Installationsanleitung folgen: https://ubuntu.com/tutorials/install-the-arduino-ide#2-installing-via-a-tarball

3. Arduino anschließen

4. Arduino IDE starten mit `sudo arduino`

5. Unter Werkzeuge->Board "Arduino Uno" wählen und unter Werkzeuge->Port "/dev/ttyUSB0"

6. Um die Funktionalität zu testen, ein Testskript ausführen: Datei->Beispiele->01 Basics->Blink, dann "Hochladen" drücken. Die "L" LED (PIN 13) sollte aufblinken.