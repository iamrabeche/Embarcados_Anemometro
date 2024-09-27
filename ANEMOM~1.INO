#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define pinHallSensor 2   // Pino digital para o sensor Hall 
#define SCREEN_WIDTH 128   // Largura do display OLED
#define SCREEN_HEIGHT 64   // Altura do display OLED
#define MAX_SAMPLES 20     // Número máximo de amostras

// Instancia o objeto do OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Definições de pinos e constantes
const int pinDirecao = A0;     // Pino analógico para direção do vento
const int raioAnemometro = 147; // Raio do anemômetro em mm
const int periodo = 5000;      // Tempo de medição em milissegundos
const int tempoEspera = 2000;  // Tempo entre amostras em milissegundos

// Constante para o valor de Pi
const float pi = 3.14159265;

// Variáveis globais
unsigned int contador = 0;
unsigned int RPM = 0;
unsigned int amostra = 0;      // Contador de amostras
float velocidadeVentoMS = 0;
float velocidadeVentoKMH = 0;

// Array para armazenar as últimas 20 amostras
float amostras[MAX_SAMPLES] = {0};

// Funções para configurar e rodar o código
void setup() {
  pinMode(pinHallSensor, INPUT_PULLUP);  // Pino do sensor Hall com pull-up ativo
  Serial.begin(9600);                    // Inicializa comunicação serial

  // Inicializa o display OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Usando o endereço 0x3C
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  amostra++;                // Incrementa o número de amostras
  Serial.print("\nAmostra ");
  Serial.print(amostra);
  Serial.println(": Iniciando medição...");

  medirVelocidadeVento();   // Medição de velocidade do vento
  exibirResultados();       // Exibe as informações de velocidade e direção
  atualizarGrafico();      // Atualiza o gráfico de velocidade

  Serial.println("Medição finalizada.");
  delay(tempoEspera);
}

// Função para medir a velocidade do vento
void medirVelocidadeVento() {
  contador = 0;
  attachInterrupt(digitalPinToInterrupt(pinHallSensor), incrementarContador, RISING);
  delay(periodo);
  detachInterrupt(digitalPinToInterrupt(pinHallSensor));

  RPM = (contador * 60) / (periodo / 1000);   // Calcula RPM
  velocidadeVentoMS = calcularVelocidadeMS(); // Calcula velocidade em m/s
  velocidadeVentoKMH = velocidadeVentoMS * 3.6; // Converte para km/h

  // Armazena a nova amostra, deslocando as mais antigas
  for (int i = MAX_SAMPLES - 1; i > 0; i--) {
    amostras[i] = amostras[i - 1];
  }
  amostras[0] = velocidadeVentoKMH; // Adiciona nova amostra na posição 0
}

// Função para exibir os resultados
void exibirResultados() {
  Serial.print("RPM: ");
  Serial.print(RPM);
  Serial.print("; Velocidade do vento: ");
  Serial.print(velocidadeVentoMS);
  Serial.print(" m/s, ");
  Serial.print(velocidadeVentoKMH);
  Serial.println(" km/h");

  exibirDirecaoVento();  // Exibe a direção do vento
}

// Função para calcular a velocidade em m/s
float calcularVelocidadeMS() {
  return (4 * pi * raioAnemometro * RPM) / 60000.0; // Velocidade do vento em m/s
}

// Função para exibir a direção do vento
void exibirDirecaoVento() {
  float valor = analogRead(pinDirecao) * (5.0 / 1023.0); // Leitura do sensor de direção

  int direcaoGraus;
  String direcaoNome;

  if (valor <= 0.27) { direcaoGraus = 315; direcaoNome = "Noroeste"; }
  else if (valor <= 0.32) { direcaoGraus = 270; direcaoNome = "Oeste"; }
  else if (valor <= 0.38) { direcaoGraus = 225; direcaoNome = "Sudoeste"; }
  else if (valor <= 0.45) { direcaoGraus = 180; direcaoNome = "Sul"; }
  else if (valor <= 0.57) { direcaoGraus = 135; direcaoNome = "Sudeste"; }
  else if (valor <= 0.75) { direcaoGraus = 90; direcaoNome = "Leste"; }
  else if (valor <= 1.25) { direcaoGraus = 45; direcaoNome = "Nordeste"; }
  else { direcaoGraus = 0; direcaoNome = "Norte"; }

  Serial.print("Direção do vento: ");
  Serial.print(direcaoGraus);
  Serial.print(" graus, ");
  Serial.println(direcaoNome);
}

// Função de interrupção para incrementar o contador
void incrementarContador() {
  contador++;
}

// Função para atualizar o gráfico de velocidade
void atualizarGrafico() {
  display.clearDisplay();  // Limpa o display

  // Exibe as leituras acima do gráfico
  display.setTextSize(1);
  display.setCursor(0, 0); // Posição do cursor no canto superior esquerdo
  display.print("RPM: ");
  display.print(RPM);
  display.print(" Vel. Vento: ");
  display.print(velocidadeVentoKMH);
  display.print(" km/h");

  // Desenha o gráfico
  for (int i = 1; i < MAX_SAMPLES; i++) {
    int x1 = map(i - 1, 0, MAX_SAMPLES - 1, 0, SCREEN_WIDTH - 1);
    int y1 = map(amostras[i - 1], 0, 100, SCREEN_HEIGHT - 1, 0); // Mapeia para a altura do display
    int x2 = map(i, 0, MAX_SAMPLES - 1, 0, SCREEN_WIDTH - 1);
    int y2 = map(amostras[i], 0, 100, SCREEN_HEIGHT - 1, 0); // Mapeia para a altura do display

    display.drawLine(x1, y1, x2, y2, WHITE); // Desenha a linha entre as amostras
  }

  // Exibe o gráfico
  display.display();
}

