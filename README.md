# Afinador d'Instruments amb ESP32 i INMP441

## Objectiu del Projecte

Aquest projecte té com a objectiu desenvolupar un **afinador d'instruments senzill** utilitzant una placa **ESP32** (compatible amb I2S) i un micròfon digital **INMP441**. El sistema és capaç de detectar la **freqüència fonamental** d'un senyal d'àudio en temps real i mostrar-la juntament amb la **nota musical** més propera, facilitant l'afinació d'instruments com la guitarra.

---

## Components Utilitzats

* Placa **ESP32** (provat amb ESP32-S3, però compatible amb altres models amb suport I2S)
* Micròfon digital **INMP441** (interfície I2S)
* Cables jumper

---

## Descripció del Funcionament

1.  El micròfon digital **INMP441** capta el so ambiental (per exemple, el d'una corda de guitarra polsada).
2.  L'**ESP32** rep les dades d'àudio en cru a través del protocol **I2S** a una freqüència de mostreig de **44.1 kHz**.
3.  El senyal d'àudio es processa en temps real utilitzant la **Transformada Ràpida de Fourier (FFT)** per calcular el seu espectre de freqüència.
4.  S'identifica la **freqüència dominant** (pic de magnitud) dins d'un rang de monitorització específic (**20 Hz a 440 Hz**).
5.  La freqüència detectada es compara amb una taula de **freqüències estàndard de notes de guitarra** (Mi, La, Re, Sol, Si, mi) per identificar la nota musical més propera.
6.  Els resultats (freqüència, magnitud del pic i nota musical) es mostren tant al **Monitor Serie** com en una **interfície web accessible** a través de Wi-Fi.

---

## Estructura del Projecte

El projecte consta d'un únic arxiu `.ino` que inclou tota la lògica de captura d'àudio, processament FFT, detecció de freqüència i la implementació del servidor web.

* **`main.ino`** (o el nom del teu sketch):
    * **Configuració I2S:** Inicialitza la comunicació amb el micròfon INMP441.
    * **Processament de Senyal:** Implementa la rutina de lectura d'àudio, pre-processament (eliminació d'offset DC), aplicació de finestra (Hamming) i càlcul de FFT.
    * **Detecció de Freqüència:** Algorisme per trobar el pic de freqüència dominant.
    * **Comparació de Notes:** Funció per identificar la nota de guitarra més propera a la freqüència detectada.
    * **Servidor Web:** Configuració de la connectivitat Wi-Fi i un servidor web HTTP.
    * **Interfície HTML/JavaScript:** Codi incrustat al sketch per generar una pàgina web dinàmica que mostra els resultats d'afinació en temps real, actualitzant-se cada **750 mil·lisegons**.

---

## Possibles Millores

* **Indicador Visual d'Afinació:** Implementar una barra o agulla virtual a la interfície web per indicar si la nota està "sostinguda" o "bemoll".
* **Detecció d'Altres Notes/Instruments:** Expandir la taula de notes per cobrir un rang més ampli o diferents instruments.
* **Calibració Automàtica:** Afegir una funció de calibració per ajustar la freqüència de referència.
* **Filtrat de Soroll:** Incorporar algorismes més avançats per reduir l'impacte del soroll ambiental en la detecció de freqüència.

---

## 📷 Exemple de Connexió (ESP32 a INMP441)

| INMP441 Pin | ESP32 Pin |
| :---------- | :-------- |
| VCC         | 3.3V      |
| GND         | GND       |
| WS (LRCL)   | GPIO2     |
| SCK         | GPIO18    |
| SD          | GPIO35    |

## Vista desde la pagina web
![image](https://github.com/user-attachments/assets/2651454b-8383-4e6b-8c10-e82b3cef092b)
