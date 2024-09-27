#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define pinSensorHall 2      // Pino digital para o sensor Hall
#define LARGURA_TELA 128     // Largura do display OLED
#define ALTURA_TELA 64       // Altura do display OLED
#define MAX_AMOSTRAS 20      // Número máximo de amostras
#define pinDirecaoVento A0   // Pino analógico para direção do vento

// Instancia o objeto do OLED
Adafruit_SSD1306 display(LARGURA_TELA, ALTURA_TELA, &Wire, -1);

// Definições de constantes
const int raioAnemometro = 147;   // Raio do anemômetro em mm
const int tempoMedicao = 5000;     // Tempo de medição em milissegundos
const int tempoEspera = 1500;      // Tempo entre amostras em milissegundos
const float pi = 3.14159265;       // Constante Pi

// Variáveis globais
unsigned int contador = 0;          // Contador de pulsos do sensor Hall
unsigned int RPM = 0;                // Rotações por minuto
float velVentoKMH = 0;               // Velocidade do vento em km/h
float amostras[MAX_AMOSTRAS] = { 0 }; // Array para armazenar amostras de velocidade

// Variáveis para controle da exibição
bool exibirGrafico = true;           // Inicia com a exibição do gráfico

void setup() {
  pinMode(pinSensorHall, INPUT_PULLUP);  // Pino do sensor Hall com pull-up ativo
  Serial.begin(9600);                     // Inicializa comunicação serial

  // Inicializa o display OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Usando o endereço 0x3C
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  medirVelocidadeVento();                // Medir a velocidade do vento
  atualizarGrafico();                    // Atualizar gráfico no display
  delay(tempoEspera);                    // Esperar antes da próxima medição
}

// Função para medir a velocidade do vento
void medirVelocidadeVento() {
  exibirDirecaoVento();                  // Exibir direção do vento

  contador = 0;                          // Resetar contador
  attachInterrupt(digitalPinToInterrupt(pinSensorHall), incrementarContador, RISING); // Configurar interrupção
  delay(tempoMedicao);                   // Esperar o tempo de medição
  detachInterrupt(digitalPinToInterrupt(pinSensorHall)); // Desconectar interrupção

  RPM = (contador * 60) / (tempoMedicao / 1000); // Calcular RPM
  velVentoKMH = calcularVelocidadeKMH(); // Calcular velocidade em km/h

  armazenarAmostra(velVentoKMH);        // Armazenar nova amostra
}

// Função para calcular a velocidade em km/h
float calcularVelocidadeKMH() {
  return (4 * pi * raioAnemometro * RPM) / 60000.0 * 3.6;  // Velocidade do vento em km/h
}

// Função para armazenar amostra de velocidade
void armazenarAmostra(float novaAmostra) {
  // Deslocar as amostras antigas
  for (int i = MAX_AMOSTRAS - 1; i > 0; i--) {
    amostras[i] = amostras[i - 1];
  }
  amostras[0] = novaAmostra; // Armazenar nova amostra
}

// Função para atualizar o gráfico de velocidade
void atualizarGrafico() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("RPM: ");
  display.print(RPM);
  display.print(" Vel. Vento: ");
  display.print(velVentoKMH);
  display.print(" km/h");

  // Desenhar o gráfico
  for (int i = 1; i < MAX_AMOSTRAS; i++) {
    int x1 = map(i - 1, 0, MAX_AMOSTRAS - 1, 0, LARGURA_TELA - 1);
    int y1 = map(amostras[i - 1], 0, 100, ALTURA_TELA - 1, 0);
    int x2 = map(i, 0, MAX_AMOSTRAS - 1, 0, LARGURA_TELA - 1);
    int y2 = map(amostras[i], 0, 100, ALTURA_TELA - 1, 0);
    display.drawLine(x1, y1, x2, y2, WHITE);
  }
  display.display(); // Atualiza o display
}

// Função para exibir a direção do vento (bússola)
void exibirDirecaoVento() {
  float valor = analogRead(pinDirecaoVento) * (5.0 / 1023.0); // Lê valor do sensor
  int direcaoGraus = obterDirecao(valor); // Obtém direção em graus
  Serial.print("Direção do vento: ");
  Serial.print(direcaoGraus);
  Serial.println(" graus");

  // Desenha a bússola no display
  desenharBussola(direcaoGraus);
}

// Função para obter a direção do vento em graus
int obterDirecao(float valor) {
  if (valor <= 0.27) return 315;   // Noroeste
  if (valor <= 0.32) return 270;   // Oeste
  if (valor <= 0.38) return 225;   // Sudoeste
  if (valor <= 0.45) return 180;   // Sul
  if (valor <= 0.57) return 135;   // Sudeste
  if (valor <= 0.75) return 90;    // Leste
  if (valor <= 1.25) return 45;    // Nordeste
  return 0;                         // Norte
}

// Função para desenhar a bússola com a seta da direção do vento
void desenharBussola(int direcao) {
  display.clearDisplay();
  display.drawCircle(100, 32, 20, WHITE);  // Círculo da bússola
  display.fillCircle(100, 32, 2, WHITE);   // Centro

  // Adiciona círculos nas direções cardeais e intermediárias
  adicionarDirecoes();

  // Desenha a seta que indica a direção do vento
  int x = 100 + 20 * cos(radians(direcao - 90));
  int y = 32 + 20 * sin(radians(direcao - 90));
  display.drawLine(100, 32, x, y, WHITE); // Seta

  // Adiciona o ângulo em graus no canto esquerdo
  display.setCursor(0, 32);
  display.setTextSize(2);
  display.print(direcao);
  display.setTextSize(1);
  display.print("o");

  display.display();  // Atualiza o display
}

// Função para adicionar direções cardeais e intermediárias
void adicionarDirecoes() {
  // Adiciona pontos cardeais
  display.fillCircle(120, 32, 1, WHITE);   // E
  display.fillCircle(80, 32, 1, WHITE);    // O
  display.fillCircle(100, 12, 1, WHITE);   // N
  display.fillCircle(100, 52, 1, WHITE);   // S
  display.fillCircle(110, 22, 1, WHITE);   // NE
  display.fillCircle(110, 42, 1, WHITE);   // SE
  display.fillCircle(90, 22, 1, WHITE);    // NW
  display.fillCircle(90, 42, 1, WHITE);    // SW

  // Adiciona os pontos cardeais de maneira organizada
  display.setCursor(96, 4);   display.print("N");
  display.setCursor(120, 30);  display.print("E");
  display.setCursor(96, 56);   display.print("S");
  display.setCursor(64, 30);   display.print("W");

  // Adiciona os pontos intermediários
  display.setCursor(114, 14);  display.print("NE");
  display.setCursor(114, 44);  display.print("SE");
  display.setCursor(64, 14);   display.print("NW");
  display.setCursor(60, 44);   display.print("SW");
}

// Função de interrupção para incrementar o contador
void incrementarContador() {
  contador++;
}

