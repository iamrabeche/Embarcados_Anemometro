#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C  // Endereço I2C do display OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define pinHallSensor 2   // Pino digital para o sensor Hall

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

// Funções para configurar e rodar o código
void setup() {
  pinMode(pinHallSensor, INPUT_PULLUP);  // Pino do sensor Hall com pull-up ativo
  Serial.begin(9600);                    // Inicializa comunicação serial
  display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
}

void loop() {
  amostra++;                // Incrementa o número de amostras
  Serial.print("\nAmostra ");
  Serial.print(amostra);
  Serial.println(": Iniciando medição...");

  medirVelocidadeVento();   // Medição de velocidade do vento
  exibirResultados();       // Exibe as informações de velocidade e direção

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

  // Desenha a rosa dos ventos com a seta
  drawCompass(direcaoGraus);
}

// Função de interrupção para incrementar o contador
void incrementarContador() {
  contador++;
}

// Função para desenhar a rosa dos ventos e a seta
void drawCompass(int direcao) {
  display.clearDisplay();

  // Círculo externo menor deslocado para a direita
  display.drawCircle(100, 32, 20, WHITE); // Mova a bússola para a direita

  // Adiciona círculos menores nas direções cardeais e intermediárias
  display.fillCircle(100, 32, 2, WHITE); // Centro
  display.fillCircle(120, 32, 1, WHITE); // E
  display.fillCircle(80, 32, 1, WHITE); // O
  display.fillCircle(100, 12, 1, WHITE); // N
  display.fillCircle(100, 52, 1, WHITE); // S
  display.fillCircle(110, 22, 1, WHITE); // NE
  display.fillCircle(110, 42, 1, WHITE); // SE
  display.fillCircle(90, 22, 1, WHITE); // NW
  display.fillCircle(90, 42, 1, WHITE); // SW

  // Adiciona os pontos cardeais de maneira organizada
  display.setCursor(96, 4);  // N
  display.print("N");
  display.setCursor(120, 30); // E
  display.print("E");
  display.setCursor(96, 56); // S
  display.print("S");
  display.setCursor(64, 30); // W
  display.print("W");

  // Adiciona os pontos intermediários
  display.setCursor(114, 14); // NE
  display.print("NE");
  display.setCursor(114, 44); // SE
  display.print("SE");
  display.setCursor(64, 14); // NW
  display.print("NW");
  display.setCursor(60, 44); // SW
  display.print("SW");

  // Desenha a seta que indica a direção do vento
  int x = 100 + 20 * cos(radians(direcao - 90)); // Corrigido
  int y = 32 + 20 * sin(radians(direcao - 90)); // Corrigido
  display.drawLine(100, 32, x, y, WHITE); // Seta

  // Adiciona o ângulo em graus do lado esquerdo
  display.setCursor(0, 32); // Nova posição para o grau à esquerda
  display.setTextSize(2); // Tamanho do texto
  display.print(direcao);
  display.setTextSize(1); // Tamanho do texto
  display.print("o");

  display.display(); // Atualiza o display
}

