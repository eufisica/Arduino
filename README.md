# eufisica/Arduino — Projetos Práticos de Física com Arduino

Este documento é baseado na estrutura e repositório **eufisica/Arduino**, focado no desenvolvimento de projetos práticos, aquisição de dados e automação de experimentos físicos utilizando a plataforma **Arduino**.

---

## 1. Visão Geral

O uso do Arduino no ensino e na pesquisa de Física permite transformar experimentos tradicionais em sistemas automatizados de coleta de dados em tempo real. Com sensores acessíveis e programação em C/C++, é possível mensurar grandezas físicas com boa precisão e baixo custo.

### Principais Aplicações:
* **Laboratório de Ensino:** Automatização de medições de tempo, temperatura, luz e movimento.
* **Aquisição de Dados:** Leitura contínua de sensores analógicos/digitais e exportação via Serial.
* **Controle e Atuação:** Controle de motores, emissão de sinais e calibração de sistemas.

---

## 2. Componentes e Sensores Comuns

| Categoria | Sensores / Módulos | Grandeza Medida / Aplicação |
| :--- | :--- | :--- |
| **Cinemática & Dinâmica** | HC-SR04 (Ultrassônico), Encoders | Posição, velocidade, aceleração, tempo de queda |
| **Termodinâmica** | DS18B20, DHT11/DHT22, Termopares | Temperatura, umidade, calorimetria |
| **Eletromagnetismo** | Sensores de Efeito Hall (ACS712) | Campo magnético, corrente elétrica |
| **Óptica & Fotometria** | LDR, Sensores de Luz (BH1750) | Intensidade luminosa, atenuação de luz |
| **Interface & Armazenamento** | Módulo SD Card, LCD 16x2, Bluetooth | Registro offline de dados e exibição |

---

## 3. Exemplos de Projetos

### A. Cronômetro de Precisão para Queda Livre
* **Objetivo:** Determinar a aceleração da gravidade $g$.
* **Funcionamento:** Um eletroímã solta uma esfera metálica enquanto ativa um cronômetro. Um sensor óptico ou de impacto na base envia um sinal de parada ao Arduino.
* **Equação Base:** 
  $$h = \frac{1}{2}g t^2 \implies g = \frac{2h}{t^2}$$

### B. Mapeamento de Campo Magnético (Efeito Hall)
* **Objetivo:** Medir a variação do campo magnético ao longo do eixo de um solenoide ou ímã permanente.
* **Funcionamento:** O sensor Hall linear envia uma variação de tensão proporcional à intensidade do campo magnético $\vec{B}$, exibida em tempo real via monitor serial.

### C. Experimento de Resfriamento de Newton
* **Objetivo:** Monitorar a taxa de resfriamento de um líquido ao longo do tempo.
* **Funcionamento:** O sensor à prova d'água DS18B20 lê a temperatura a intervalos fixos ($\Delta t$), gerando uma curva exponencial de resfriamento.

---

## 4. Estrutura Padrão de Código (Template)

Um esboço básico para aquisição de dados e envio via Serial:

```cpp
// Leitura de Sensor Analogico para Experimentos de Fisica
const int sensorPin = A0;
unsigned long tempoInicial;

void setup() {
  Serial.begin(9600);
  Serial.println("Tempo(ms),LeituraRaw,Tensao(V)");
  tempoInicial = millis();
}

void loop() {
  unsigned long tempoAtual = millis() - tempoInicial;
  int valorRaw = analogRead(sensorPin);
  float tensao = valorRaw * (5.0 / 1023.0);

  // Formato CSV para exportação e análise (Python/Excel)
  Serial.print(tempoAtual);
  Serial.print(",");
  Serial.print(valorRaw);
  Serial.print(",");
  Serial.println(tensao);

  delay(100); // Frequência de amostragem (10 Hz)
}
```

---

## 5. Integração com Análise de Dados

Os dados coletados pelo Arduino via comunicação Serial podem ser processados por scripts em **Python** (usando `pySerial`, `pandas` e `matplotlib`) para ajuste de curvas, cálculo de incertezas e plotagem de gráficos em tempo real.
