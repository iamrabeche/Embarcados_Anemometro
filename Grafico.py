import serial
import matplotlib.pyplot as plt
import numpy as np

# Configurações da porta serial
ser = serial.Serial('COM3', 9600)  # Substitua 'COM3' pela porta correta
ser.flush()

# Listas para armazenar os dados
velocidades = []
direcoes = []

# Direções cardeais e intermediárias
direcoes_cardinais = {
    0: 'N',
    45: 'NE',
    90: 'L',
    135: 'SE',
    180: 'S',
    225: 'SO',
    270: 'O',
    315: 'NO'
}

# Cria a figura e o eixo para o gráfico
plt.figure(figsize=(10, 6))
plt.ion()  # Ativa o modo interativo

try:
    while True:
        if ser.in_waiting > 0:
            linha = ser.readline().decode('utf-8').rstrip()
            dados = linha.split(",")
            if len(dados) == 2:
                velocidade = float(dados[0])
                direcao = int(dados[1])
                velocidades.append(velocidade)

                # Atualiza gráfico da velocidade do vento
                plt.clf()  # Limpa a figura
                plt.plot(velocidades, label='Velocidade do Vento (km/h)', color='orange')
                plt.xlabel('Amostra')
                plt.ylabel('Velocidade (km/h)')
                plt.title('Velocidade do Vento')
                plt.legend()

                # Converte a direção para a direção cardeal mais próxima
                direcao_cardinal = direcoes_cardinais.get((direcao // 45) * 45, f'{direcao}°')

                # Exibe a direção do vento
                plt.text(0.5, 0.5, f'Direção do Vento: {direcao_cardinal} ({direcao}°)', fontsize=20, ha='center', va='center')
                plt.axis('off')  # Remove os eixos para melhor visualização

                plt.pause(0.1)  # Pausa para atualização

except KeyboardInterrupt:
    print("Saindo...")
finally:
    ser.close()
