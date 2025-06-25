#include <Arduino.h>
#include <driver/i2s.h>
#include "arduinoFFT.h"
#include <WiFi.h>
#include <WebServer.h>

// --- Configuración Wi-Fi ---
const char* ssid = "Mariajuana's phone";
const char* password = "AmarilloPlatano";

// --- Pines I2S ---
#define I2S_WS_PIN 2
#define I2S_SCK_PIN 18
#define I2S_SD_PIN 35
#define I2S_NUM I2S_NUM_0

#define SAMPLES 4096
#define SAMPLE_BUFFER_SIZE (SAMPLES * sizeof(int32_t))

int32_t raw_samples[SAMPLES];
double vReal[SAMPLES];
double vImag[SAMPLES];
ArduinoFFT<double> FFT;

const double samplingFrequency = 44100;
const double MIN_FREQ_DISPLAY = 20.0;
const double MAX_FREQ_DISPLAY = 440.0;
const unsigned long FREQUENCY_DISPLAY_INTERVAL = 750;

unsigned long lastFrequencyDisplayTime = 0;

WebServer server(80);
double currentDetectedFrequency = 0.0;
double currentPeakMagnitude = 0.0;
String currentNote = "---";

// --- Notas de guitarra ---
struct Note {
  const char* name;
  double frequency;
};

Note guitarNotes[] = {
  {"E", 82.41},
  {"A", 110.00},
  {"D", 146.83},
  {"G", 196.00},
  {"B", 246.94},
  {"e", 329.63}
};

const char* getClosestGuitarNote(double frequency) {
  const char* closestNote = "---";
  double minDiff = 1e6;
  for (int i = 0; i < sizeof(guitarNotes) / sizeof(Note); i++) {
    double diff = abs(frequency - guitarNotes[i].frequency);
    if (diff < minDiff) {
      minDiff = diff;
      closestNote = guitarNotes[i].name;
    }
  }
  return closestNote;
}

// --- Página HTML ---
const char* HTML_PAGE = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Monitor de Frecuencia INMP441</title>
<style>
  body { font-family: Arial, sans-serif; text-align: center; margin: 50px; background-color: #f0f0f0; color: #333; }
  .container { background-color: #fff; padding: 30px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); display: inline-block; }
  h1 { color: #0056b3; }
  p { font-size: 1.2em; }
  #frequencyDisplay { font-size: 3em; font-weight: bold; color: #28a745; margin-top: 20px; }
  #magnitudeDisplay { font-size: 1.5em; color: #6c757d; }
  #noteDisplay { font-size: 2em; color: #007bff; margin-top: 10px; }
  .status { font-size: 0.9em; color: #888; margin-top: 20px;}
</style>
</head>
<body>
<div class="container">
  <h1>Frecuencia Detectada</h1>
  <p>Rango de monitoreo: 20 Hz - 440 Hz</p>
  <div id="frequencyDisplay">--.-- Hz</div>
  <div id="noteDisplay">Nota: ---</div>
  <div id="magnitudeDisplay">(Magnitud: --.--)</div>
  <p class="status">Actualizando cada ~0.75 segundos...</p>
</div>

<script>
function fetchData() {
  fetch('/data')
    .then(response => response.json())
    .then(data => {
      document.getElementById('frequencyDisplay').innerText = data.frequency.toFixed(2) + ' Hz';
      document.getElementById('magnitudeDisplay').innerText = '(Magnitud: ' + data.magnitude.toFixed(2) + ')';
      document.getElementById('noteDisplay').innerText = 'Nota: ' + data.note;
    })
    .catch(error => {
      console.error('Error al obtener datos:', error);
      document.getElementById('frequencyDisplay').innerText = 'ERROR';
      document.getElementById('magnitudeDisplay').innerText = '';
      document.getElementById('noteDisplay').innerText = 'Nota: ---';
    });
}

setInterval(fetchData, 750);
fetchData();
</script>
</body>
</html>
)rawliteral";

// --- Servidor Web ---
void handleRoot() {
  server.send(200, "text/html", HTML_PAGE);
}

void handleData() {
  String jsonResponse = "{";
  jsonResponse += "\"frequency\": " + String(currentDetectedFrequency, 2);
  jsonResponse += ", \"magnitude\": " + String(currentPeakMagnitude, 2);
  jsonResponse += ", \"note\": \"" + currentNote + "\"";
  jsonResponse += "}";
  server.send(200, "application/json", jsonResponse);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando sistema de frecuencia...");

  // Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado. IP: " + WiFi.localIP().toString());

  // Servidor web
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor web iniciado.");

  // I2S
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = (uint32_t)samplingFrequency,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD_PIN
  };

  if (i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL) != ESP_OK) {
    Serial.println("Error instalando I2S");
    while (true);
  }
  if (i2s_set_pin(I2S_NUM, &pin_config) != ESP_OK) {
    Serial.println("Error configurando pines I2S");
    while (true);
  }

  Serial.printf("FFT: resolución %.2f Hz, rango %.2f-%.2f Hz\n", samplingFrequency / SAMPLES, MIN_FREQ_DISPLAY, MAX_FREQ_DISPLAY);
}

void loop() {
  server.handleClient();

  size_t bytes_read;
  if (i2s_read(I2S_NUM, (char *)raw_samples, SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY) != ESP_OK) return;

  if (bytes_read / sizeof(int32_t) != SAMPLES) return;

  if (millis() - lastFrequencyDisplayTime >= FREQUENCY_DISPLAY_INTERVAL) {
    lastFrequencyDisplayTime = millis();

    long long sum_samples = 0;
    for (int i = 0; i < SAMPLES; i++) sum_samples += (raw_samples[i] >> 8);
    double average_sample = (double)sum_samples / SAMPLES;

    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = (double)(raw_samples[i] >> 8) - average_sample;
      vImag[i] = 0.0;
    }

    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.complexToMagnitude(vReal, vImag, SAMPLES);

    double peak_magnitude = 0;
    int peak_index = 0;
    int start_index = max(1, (int)(MIN_FREQ_DISPLAY / (samplingFrequency / SAMPLES)));
    int end_index = min((int)(MAX_FREQ_DISPLAY / (samplingFrequency / SAMPLES)), SAMPLES / 2);

    for (int i = start_index; i < end_index; i++) {
      if (vReal[i] > peak_magnitude) {
        peak_magnitude = vReal[i];
        peak_index = i;
      }
    }

    double frequency = (double)peak_index * samplingFrequency / SAMPLES;

    if (frequency >= MIN_FREQ_DISPLAY && frequency <= MAX_FREQ_DISPLAY) {
      currentDetectedFrequency = frequency;
      currentPeakMagnitude = peak_magnitude;
      currentNote = getClosestGuitarNote(frequency);
      Serial.printf("Frecuencia: %.2f Hz, Magnitud: %.2f, Nota: %s\n", frequency, peak_magnitude, currentNote.c_str());
    } else {
      currentDetectedFrequency = 0.0;
      currentPeakMagnitude = 0.0;
      currentNote = "---";
    }
  }
}
