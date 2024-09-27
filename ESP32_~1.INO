#define pinHallSensor 2  // Pino digital para o sensor Hall (GPIO4 no ESP8266)

// Definições de pinos e constantes
const int pinDirecao = 36;     // Pino analógico para direção do vento
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
}

// Função de interrupção para incrementar o contador
void incrementarContador() {
  contador++;
}
