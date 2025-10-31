#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración LCD I2C (usar dirección 0x27 o 0x3F en Tinkercad)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pines para el L293D (Motor DC - Cinta)
const int EN1_motor = 10;  // Enable motor cinta (PWM)
const int IN1_motor = 8;   // Control dirección 1
const int IN2_motor = 9;   // Control dirección 2

// Pines para el L293D (Bomba de agua)
const int EN2_bomba = 6;   // Enable bomba (PWM)
const int IN3_bomba = 5;   // Control dirección 3
const int IN4_bomba = 4;   // Control dirección 4

// Sensores infrarrojos (en Tinkercad son HIGH cuando detectan)
const int sensor1 = 2;     // Sensor de inicio
const int sensor2 = 3;     // Sensor final

// LEDs y pulsador
const int ledRojo = 11;    // Bomba activa
const int ledVerde = 12;   // Cinta activa
const int pulsador = 7;    // Pausa/inicio (pull-up interno)

// Variables de estado
bool sistemaActivo = true;
int contadorBomba = 0;
unsigned long ultimoCambio = 0;
const int debounceDelay = 200;

void setup() {
  // Configurar pines de control motor
  pinMode(EN1_motor, OUTPUT);
  pinMode(IN1_motor, OUTPUT);
  pinMode(IN2_motor, OUTPUT);
  
  // Configurar pines de control bomba
  pinMode(EN2_bomba, OUTPUT);
  pinMode(IN3_bomba, OUTPUT);
  pinMode(IN4_bomba, OUTPUT);
  
  // Configurar LEDs y pulsador
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(pulsador, INPUT_PULLUP);
  
  // Configurar sensores (en Tinkercad no necesitan pull-up)
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  
  // Iniciar LCD (en Tinkercad a veces requiere reinicio)
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Iniciando...");
  delay(1000);  // Pausa para estabilizar LCD
  
  // Iniciar sistema
  activarMotorDC();
  actualizarLCD();
}

void loop() {
  // Manejo del pulsador con antirrebote
  if (digitalRead(pulsador) == LOW && (millis() - ultimoCambio) > debounceDelay) {
    sistemaActivo = !sistemaActivo;
    ultimoCambio = millis();
    
    if (sistemaActivo) {
      activarMotorDC();
      lcd.clear();
      lcd.print("Sistema ACTIVO");
    } else {
      detenerMotorDC();
      detenerBombaAgua();
      lcd.clear();
      lcd.print("Sistema PAUSADO");
    }
  }

  if (sistemaActivo) {
    // Leer sensores (en Tinkercad son HIGH al detectar)
    bool objetoDetectado1 = digitalRead(sensor1);
    bool objetoDetectado2 = digitalRead(sensor2);
    
    // Lógica principal
    if (objetoDetectado1 && !objetoDetectado2) {
      detenerMotorDC();
      delay(3000);
      
      activarBombaAgua();
      contadorBomba++;
      delay(4000);
      
      detenerBombaAgua();
      delay(2000);
      
      activarMotorDC();
      actualizarLCD();
    }
    
    if (objetoDetectado2) {
      detenerMotorDC();
      delay(1000);
      activarMotorDC();
      actualizarLCD();
    }
  }
}

// Funciones de control mejoradas para Tinkercad
void activarMotorDC() {
  digitalWrite(IN1_motor, HIGH);
  digitalWrite(IN2_motor, LOW);
  analogWrite(EN1_motor, 150);  // Velocidad reducida para simulación
  digitalWrite(ledVerde, HIGH);
}

void detenerMotorDC() {
  digitalWrite(IN1_motor, LOW);
  digitalWrite(IN2_motor, LOW);
  analogWrite(EN1_motor, 0);
  digitalWrite(ledVerde, LOW);
}

void activarBombaAgua() {
  digitalWrite(IN3_bomba, HIGH);
  digitalWrite(IN4_bomba, LOW);
  analogWrite(EN2_bomba, 200);  // Potencia media para simulación
  digitalWrite(ledRojo, HIGH);
}

void detenerBombaAgua() {
  digitalWrite(IN3_bomba, LOW);
  digitalWrite(IN4_bomba, LOW);
  analogWrite(EN2_bomba, 0);
  digitalWrite(ledRojo, LOW);
}

void actualizarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cinta:");
  lcd.print(digitalRead(ledVerde) ? "ON " : "OFF");
  lcd.print(" Bomba:");
  lcd.print(digitalRead(ledRojo) ? "ON" : "OFF");
  
  lcd.setCursor(0, 1);
  lcd.print("Ciclos:");
  lcd.print(contadorBomba);
}
