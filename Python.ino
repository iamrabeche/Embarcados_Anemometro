#define pinHallSensor 2   // Pino digital para o sensor Hall

// Definições de pinos e constantes
const int pinDirecao = A0;     // Pino analógico para direção do vento
const int raioAnemometro = 147; // Raio do anemômetro em mm
const int periodo = 1000;      // Tempo de medição em milissegundos
const int tempoEspera = 200;   // Tempo entre amostras em milissegundos

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
}

void loop() {
  amostra++;                // Incrementa o número de amostras
  medirVelocidadeVento();  // Medição de velocidade do vento
  int direcaoGraus = exibirDirecaoVento(); // Captura a direção do vento

  // Envia os dados para o Serial em formato CSV
  Serial.print(velocidadeVentoKMH);
  Serial.print(",");
  Serial.println(direcaoGraus);

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

// Função para calcular a velocidade em m/s
float calcularVelocidadeMS() {
  return (4 * pi * raioAnemometro * RPM) / 60000.0; // Velocidade do vento em m/s
}

// Função para exibir a direção do vento e retornar a direção em graus
int exibirDirecaoVento() {
  float valor = analogRead(pinDirecao) * (5.0 / 1023.0); // Leitura do sensor de direção

  int direcaoGraus;

  if (valor <= 0.27) { direcaoGraus = 315; }
  else if (valor <= 0.32) { direcaoGraus = 270; }
  else if (valor <= 0.38) { direcaoGraus = 225; }
  else if (valor <= 0.45) { direcaoGraus = 180; }
  else if (valor <= 0.57) { direcaoGraus = 135; }
  else if (valor <= 0.75) { direcaoGraus = 90; }
  else if (valor <= 1.25) { direcaoGraus = 45; }
  else { direcaoGraus = 0; }

  return direcaoGraus;
}

// Função de interrupção para incrementar o contador
void incrementarContador() {
  contador++;
}

