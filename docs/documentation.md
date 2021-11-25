# Dokumentation Wetterstation - LoRaWAN

## Einleitung

Das Projekt Wetterstation - LoRaWan wurde im Laufe des Projektseminars 2021 von Lenny Reitz und Raphael Rezaii-Djafari unter der Führung von Professor Jörg Vogt an der HTW Dresden entwickelt.

### LoRaWAN

### Problemstellung

### Lösungsansatz


## Einrichtung


### Hardware

* [Wettermessgerät-Kit](https://learn.sparkfun.com/tutorials/weather-meter-hookup-guide#resources-and-going-further) SEN-15901
    * Regenmesser
    * Anemometer
    * Windfahne
* Breadbord
* Jumper Kabel
* 10kOhm Widerstand
* 2 [Modular-Einbaubuchsen mit Anschlusslitzen, 6P6C
](https://www.pollin.de/p/modular-einbaubuchse-mit-anschlusslitzen-6p6c-541843)
* Arduino Uno Klon
* [Dragino Lora Shield v1.4](https://wiki.dragino.com/index.php?title=Lora_Shield)



#### Aufbau


### Software

#### genutzte Libraries

* TODO - LoRa Library


## Funktionsweise

### Sensoren

#### Regenmesser

Der Regenmesser besteht aus einem Auffangbehälter und einem darin befindlichen einfachen Kippschalter. Wenn sich genügend Wasser im Behälter gesammelt hat, kippt der Schalter um und das Wasser läuft wieder aus dem Behälter heraus. Ein Kippvorgang entspricht einer Wassermenge von 0,2794 mm. Die folgende Umrechung ergibt die Wassermenge in Milliliter:

1 mm Niederschlagsmenge = 1 

<img src="https://latex.codecogs.com/svg.latex?\Large&space;1mm=1\frac{l}{m2}" title="\Large x=\frac{-b\pm\sqrt{b^2-4ac}}{2a}" />



Mehrere Testläufe haben gezeigt, dass der Regenmesser mit relativ großer Ungenauigkeit schaltet. Je nach dem, wie schnell das Wasser einfließt, variiert die Messgenauigkeit stark. Bei einem Durchlauf mit 100 ml Wasser ergab die tatsächlich gemessene Wassermenge nach Umrechnung: 

#### Anemometer (Windgeschwindigkeit)

Das Schalenanemometer misst Windgeschwindigkeiten durch schließen des Kontaktes eines Schalters durch einen Magneten. Eine Windgeschwindigkeit von 2.4km/h schliest den Schalter einmal pro Sekunde.

Eine Umdrehung entspricht 3 Schaltvorgängen.

#### Windfahne (Windrichtung)

Die Windfahne besitzt 8 Schalter welche jeweils mit unterschiedlich großen Widerständen verbunden sind. Die Schalter könne einzeln oder in Paaren umgelegt werden wodurch sich 16 Mögliche Positionen bestimmen lassen.

Durch einen externen Widerstand wird ein Spannungsteiler implementiert, der eine messbare Spannung ausgibt.

Daher lässt sich die Windrichtung nur durch ein analoges Signal bestimmen.

### LoRa

### Beispielablauf