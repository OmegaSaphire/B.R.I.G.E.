# B.R.I.G.E.

![Ansamblu final](Imagini/Poze%20Proiect/Ansamblu%20Final.jpeg)

B.R.I.G.E. este o proteză mecatronică de antebraț controlată printr-un sistem hibrid: o mănușă cu encodere pentru mișcarea degetelor și senzori EMG pentru comenzi rapide.

Proiectul folosește trei plăci ESP32 care comunică prin Wi-Fi și WebSocket. Mănușa trimite pozițiile celor cinci degete, modulul muscular trimite comenzi pentru mână și încheietură, iar modulul de pe proteză controlează servomotoarele printr-un driver PCA9685.

## Ce poate face acum

- controlează individual cele cinci degete ale protezei;
- copiază mișcarea degetelor din mănușa cu encodere HW-040;
- închide/deschide complet mâna prin comandă EMG;
- rotește încheietura între 0° și 90°;
- funcționează fără internet, printr-o rețea locală creată de ESP32-ul de pe proteză.

## Modulele proiectului

### Glove ESP32

![Modulul Glove](Imagini/Poze%20Proiect/Modulul%20Glove.jpeg)

Mănușa are cinci encodere rotative HW-040, câte unul pentru fiecare deget. Encoderele sunt incrementale, deci sistemul pornește cu mănușa în poziția deschisă și contorii resetați la zero.

Valorile calibrate pentru glove:

| Deget | Direcție | Max steps |
|---|---:|---:|
| Deget mare | +1 | 8 |
| Arătător | -1 | 4 |
| Mijlociu | -1 | 9 |
| Inelar | +1 | 5 |
| Deget mic | +1 | 4 |

### Muscle ESP32

![Modulul Muscle](Imagini/Poze%20Proiect/Modulul%20Muscle.jpeg)

Modulul muscular folosește doi senzori EMG H124SG. Un senzor este folosit pentru comanda de mână, iar al doilea pentru comanda de wrist. Semnalul este filtrat hardware și software, apoi este comparat cu baseline-ul măsurat la pornire.

Pragurile folosite în testele actuale:

| Parametru | Valoare |
|---|---:|
| handThreshold | 350 |
| wristThreshold | 120 |
| handResetThreshold | 120 |
| wristResetThreshold | 60 |

Comanda de mână are prioritate. Dacă semnalul pentru mână depășește pragul, comanda pentru wrist este blocată temporar ca să nu apară declanșări greșite.

### Hand ESP32

![Modulul Hand](Imagini/Poze%20Proiect/Modulul%20Hand.jpeg)

Modulul Hand este montat pe proteză. El creează rețeaua Wi-Fi locală, primește datele de la celelalte două module și controlează servomotoarele prin PCA9685.

Canale folosite pe PCA9685:

| Canal | Funcție |
|---:|---|
| CH0 | Deget mare |
| CH1 | Arătător |
| CH2 | Mijlociu |
| CH3 | Inelar |
| CH4 | Deget mic |
| CH5 | Wrist |

## Calibrare servomotoare

| Element | Open / relaxat | Close / flexat |
|---|---:|---:|
| Deget mare | 20° | 124° |
| Arătător | 8° | 120° |
| Mijlociu | 178° | 48° |
| Inelar | 8° | 180° |
| Deget mic | 80° | 110° |
| Wrist | 0° | 90° |

Limitele sunt puse în cod ca protecție. Nu trimit valori 0–180° direct, pentru că mecanismul cu cabluri poate fi forțat ușor.

## Comunicație

ESP32-ul de pe proteză creează rețeaua locală:

```text
SSID: HAND_HOST
Password: 12345678
```

Mesajele trimise între module sunt simple, ca să fie ușor de verificat în Serial Monitor:

```text
G,thumb,index,middle,ring,pinky
M,1
M,2
```

- `G,...` vine de la mănușă și conține unghiurile celor cinci degete;
- `M,1` vine de la modulul EMG și face toggle pentru mână open/close;
- `M,2` face toggle pentru wrist între 0° și 90°.

## Alimentare

Modulul Hand este alimentat de la un pachet de aproximativ 12V format din 6 celule, protejat cu BMS de 60A. Tensiunea este coborâtă la 5V cu un convertor step-down pentru ESP32, PCA9685 și servomotoare.

Modulele Glove și Muscle sunt alimentate separat. Fiecare folosește câte o celulă Li-Ion/LiPo, un modul TP4056 pentru încărcare și un step-up la 5V.

## Tehnologii folosite

- ESP32
- Wi-Fi / WebSocket
- PCA9685
- servomotoare
- HW-040 rotary encoders
- senzori EMG H124SG
- Arduino IDE
- imprimare 3D


## Galerie

| Ansamblu final | Glove | Muscle |
|---|---|---|
| ![](Imagini/Poze%20Proiect/Ansamblu%20Final.jpeg) | ![](Imagini/Poze%20Proiect/Modulul%20Glove.jpeg) | ![](Imagini/Poze%20Proiect/Modulul%20Muscle.jpeg) |

| Hand | Testare | Construcție |
|---|---|---|
| ![](Imagini/Poze%20Proiect/Modulul%20Hand.jpeg) | ![](Imagini/Poze%20din%20timpul%20constructiei/Testarea%20capacitatiilor%20modulului%20hand%20de%20a%20face%20gesturi.jpeg) | ![](Imagini/Poze%20din%20timpul%20constructiei/Modulul%20Hand%20calibrat.jpeg) |

## Status

Prototipul funcționează cu cele trei module conectate simultan. Mănușa controlează degetele, iar senzorii EMG controlează mâna și wrist-ul. Următorii pași sunt mișcare mai fluidă pentru servomotoare, carcase mai compacte și înlocuirea encoderelor incrementale cu senzori absoluți.
