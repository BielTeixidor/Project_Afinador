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
3. Se procesa la señal de audio en tiempo real para calcular la **frecuencia dominante**, mediante transformada rápida de Fourier (**FFT**) u otro algoritmo de estimación de tono.
4. La frecuencia calculada se compara con las frecuencias estándar de notas musicales (por ejemplo, 440 Hz para el **LA**).
5. La frecuencia se muestra por puerto serie o en pantalla para afinar el instrumento.



## Estructura del proyecto

