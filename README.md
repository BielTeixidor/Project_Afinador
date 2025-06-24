Afinador de Instrumentos con ESP32-S3 e INMP441

## Objetivo de la práctica

El objetivo de la práctica es diseñar y programar un afinador de instrumentos sencillo utilizando una placa **ESP32-S3** y un micrófono digital **INMP441**, capaz de detectar la frecuencia fundamental de una señal de audio y mostrarla para su comparación con notas musicales estándar.



## Componentes utilizados

- Placa **ESP32-S3** (con soporte para I2S)
- Micrófono digital **INMP441** (comunicación I2S)
- Cables jumper
- (Opcional) Pantalla OLED o interfaz serie para visualizar la frecuencia



## Descripción del funcionamiento

1. El **INMP441** capta el sonido ambiental (por ejemplo, de una cuerda de guitarra).
2. El **ESP32-S3** recibe los datos de audio a través del protocolo **I2S**.
3. Se procesa la señal de audio en tiempo real para calcular la **frecuencia dominante**, mediante transformada rápida de Fourier (**FFT**).
4. La frecuencia calculada se compara con las frecuencias estándar de notas musicales (por ejemplo, 440 Hz para el **LA**).
5. La frecuencia se muestra por puerto serie o en pantalla para afinar el instrumento.



## Estructura del proyecto




## Comentarios

- El **INMP441** es un micrófono digital muy sensible y adecuado para análisis de audio, ya que ofrece buena respuesta en frecuencia.
- La elección de la **FFT** o de algoritmos más precisos como **autocorrelación** o **YIN** depende del nivel de exactitud requerido y de la carga computacional disponible.
- El ESP32-S3 es ideal para este tipo de aplicaciones gracias a su **potencia de procesamiento** y **soporte nativo de I2S**.
- Se recomienda filtrar el ruido ambiental y normalizar la señal para mejorar la precisión de la detección.



## Posibles mejoras

- Mostrar la nota musical directamente (por ejemplo, "C4", "E2", etc.).
- Añadir una barra de sintonización visual estilo afinador cromático.
- Implementar una interfaz web (mediante Wi-Fi) para mostrar el resultado en un navegador.



## 📷 Ejemplo de conexión (ESP32-S3 a INMP441)

| INMP441 Pin | ESP32-S3 Pin |
|-------------|--------------|
| VCC         | 3.3V         |
| GND         | GND          |
| WS (LRCL)   | GPIO25       |
| SCK         | GPIO26       |
| SD          | GPIO22       |

> Asegúrate de que tu micrófono está configurado correctamente como mono y con la ganancia adecuada.


