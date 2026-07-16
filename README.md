# B.R.I.G.E.

B.R.I.G.E. este o proteză mecatronică de antebraț controlată printr-un sistem hibrid format dintr-o mănușă cu encodere și senzori EMG.

Proiectul folosește trei module ESP32 care comunică wireless prin WebSocket. Mănușa controlează individual cele cinci degete, iar modulul EMG permite comenzi rapide pentru închiderea/deschiderea completă a mâinii și rotația încheieturii.

## Modulele proiectului

### 1. Glove ESP32
Modulul Glove este o mănușă pe care sunt montate cinci encodere rotative HW-040, câte unul pentru fiecare deget. Acestea trimit pozițiile degetelor către proteză.

### 2. Muscle ESP32
Modulul Muscle folosește doi senzori EMG H124SG pentru detectarea activității musculare. Un senzor controlează mâna, iar celălalt controlează încheietura.

### 3. Hand ESP32
Modulul Hand este montat pe proteză. Acesta creează rețeaua Wi-Fi locală, primește datele de la celelalte module și controlează servomotoarele prin PCA9685.

## Tehnologii folosite

- ESP32
- Wi-Fi WebSocket
- PCA9685
- Servomotoare
- Encodere HW-040
- Senzori EMG H124SG
- Arduino IDE
- Imprimare 3D

## Alimentare

Modulul Hand este alimentat de la un acumulator de aproximativ 12V, format din 6 celule și protejat printr-un BMS de 60A. Tensiunea este coborâtă la 5V cu un convertor step-down.

Modulele Glove și Muscle sunt alimentate separat, fiecare folosind câte o celulă Li-Ion/LiPo conectată la un modul de încărcare TP4056 și la un convertor step-up la 5V.

## Structura repository-ului

```text
coduri/
  hand_esp32/
  glove_esp32/
  muscle_esp32/

documentatie/
  documentatie_BRIGE.docx
  prezentare_BRIGE.pptx

imagini/
  ansamblu_BRIGE.jpg
  proteza.jpg
  manusa_encodere.jpg
  modul_EMG.jpg
