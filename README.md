# B.R.I.G.E.

![Ansamblu final](Imagini/Poze%20Proiect/Ansamblu%20Final.jpeg)

B.R.I.G.E. este o proteză mecatronică de antebraț controlată printr-un sistem hibrid format dintr-o mănușă cu encodere și senzori EMG.

Proiectul folosește trei module ESP32 care comunică wireless prin WebSocket. Mănușa controlează individual cele cinci degete, iar modulul EMG permite comenzi rapide pentru închiderea/deschiderea completă a mâinii și rotația încheieturii.

## Modulele proiectului

### 1. Glove ESP32

![Modulul Glove](Imagini/Poze%20Proiect/Modulul%20Glove.jpeg)

Modulul Glove este o mănușă pe care sunt montate cinci encodere rotative HW-040, câte unul pentru fiecare deget. Acestea citesc mișcarea degetelor și trimit pozițiile către proteză.

### 2. Muscle ESP32

![Modulul Muscle](Imagini/Poze%20Proiect/Modulul%20Muscle.jpeg)

Modulul Muscle folosește doi senzori EMG H124SG pentru detectarea activității musculare. Un senzor controlează închiderea/deschiderea completă a mâinii, iar celălalt controlează rotația încheieturii.

### 3. Hand ESP32

![Modulul Hand](Imagini/Poze%20Proiect/Modulul%20Hand.jpeg)

Modulul Hand este montat pe proteză. Acesta creează rețeaua Wi-Fi locală, primește datele de la celelalte module și controlează servomotoarele prin driverul PCA9685.

## Tehnologii folosite

- ESP32
- Wi-Fi WebSocket
- PCA9685
- Servomotoare
- Encodere rotative HW-040
- Senzori EMG H124SG
- Arduino IDE
- Imprimare 3D

## Alimentare

Modulul Hand este alimentat de la un acumulator de aproximativ 12V, format din 6 celule și protejat printr-un BMS de 60A. Tensiunea este coborâtă la 5V cu un convertor step-down.

Modulele Glove și Muscle sunt alimentate separat, fiecare folosind câte o celulă Li-Ion/LiPo conectată la un modul de încărcare TP4056 și apoi la un convertor step-up la 5V.

## Galerie proiect

| Ansamblu final | Modulul Glove | Modulul Muscle |
|---|---|---|
| ![](Imagini/Poze%20Proiect/Ansamblu%20Final.jpeg) | ![](Imagini/Poze%20Proiect/Modulul%20Glove%20de%20sus.jpeg) | ![](Imagini/Poze%20Proiect/Modulul%20Muscle%20din%20laterala.jpeg) |

| Modulul Hand | Hand de sus | Testare gesturi |
|---|---|---|
| ![](Imagini/Poze%20Proiect/Modulul%20Hand.jpeg) | ![](Imagini/Poze%20Proiect/Modulul%20Hand%20de%20sus.jpeg) | ![](Imagini/Poze%20din%20timpul%20constructiei/Testarea%20capacitatiilor%20modulului%20hand%20de%20a%20face%20gesturi.jpeg) |

## Procesul de construcție

În timpul realizării proiectului au fost testate separat alimentarea, modulul Hand, modulul Glove, modulul Muscle, traseele cablurilor și calibrarea servomotoarelor.

![Modulul Hand în calibrare](Imagini/Poze%20din%20timpul%20constructiei/Modulul%20Hand%20calibrat.jpeg)

![Modulul Muscle după cablare](Imagini/Poze%20din%20timpul%20constructiei/Modulul%20Muscle%20dupa%20cablare.jpeg)

## Coduri

Codurile proiectului sunt împărțite în trei module:

- `coduri/glove_esp32`
- `coduri/muscle_esp32`
- `coduri/hand_esp32`

Această structură permite testarea și modificarea fiecărui modul separat.

## Documentație

Documentația proiectului se află în folderul `Documentatie`.

## Demo

În folderul cu poze din timpul construcției sunt incluse și materiale video de test, folosite pentru verificarea funcționării modulului Hand și a mișcărilor protezei.

## Scopul proiectului

Scopul proiectului este realizarea unui prototip funcțional, accesibil și modular, care demonstrează cum pot fi combinate robotica, electronica, programarea, biologia și proiectarea 3D pentru dezvoltarea unei soluții practice de protezare.
