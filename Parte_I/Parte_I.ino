#include <Servo.h>
#include <EEPROM.h>

Servo servoMotor;

// VARIABLES
String comando = "";
bool leyendoSensor = false;
bool esperandoParametroServo = false;
unsigned long intervalo = 1000;
unsigned long tiempoAnterior = 0;

// =========================
// SETUP
// =========================
void setup() {
  Serial.begin(9600);
  servoMotor.attach(9);  // Pin 9 para el servo
  pinMode(A0, INPUT);     // Pin A0 para el sensor (potenciómetro)
  
  // MENSAJE GUARDADO EN EEPROM
  String mensaje = leerMensajeEEPROM();
  if (mensaje.length() > 0) {
    Serial.println("=== MENSAJE DE BIENVENIDA ===");
    Serial.println(mensaje);
    Serial.println("=============================");
  }
  
  mostrarMenu();
}

// =========================
// LOOP
// =========================
void loop() {
  // =========================
  // LECTURA CONTINUA SENSOR
  // =========================
  if (leyendoSensor) {
    // Detener con Ctrl+C (carácter 0x03) o 'x'
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'x' || c == 'X' || c == 0x03) {
        leyendoSensor = false;
        Serial.println("\n[Lectura de sensor detenida]");
        mostrarPrompt();
      }
    }
    
    // Imprimir lectura del sensor
    if (millis() - tiempoAnterior >= intervalo) {
      tiempoAnterior = millis();
      int valor = analogRead(A0);
      Serial.print("[");
      Serial.print(millis());
      Serial.print(" ms] Sensor A0: ");
      Serial.println(valor);
    }
    return;
  }
  
  // =========================
  // ESPERANDO PARÁMETRO SERVO
  // =========================
  if (esperandoParametroServo) {
    if (Serial.available()) {
      String entrada = Serial.readStringUntil('\n');
      entrada.trim();
      int angulo = entrada.toInt();
      
      if (angulo >= 0 && angulo <= 180) {
        servoMotor.write(angulo);
        Serial.print("Servo movido a ");
        Serial.print(angulo);
        Serial.println(" grados");
      } else {
        Serial.println("Error: Ángulo debe estar entre 0 y 180");
      }
      
      esperandoParametroServo = false;
      mostrarPrompt();
    }
    return;
  }
  
  // =========================
  // LEER COMANDOS
  // =========================
  if (Serial.available()) {
    comando = Serial.readStringUntil('\n');
    comando.trim();
    
    if (comando.length() > 0) {
      procesarComando(comando);
    }
  }
}

// =========================
// PROCESAR COMANDOS
// =========================
void procesarComando(String cmd) {
  
  // =========================
  // COMANDO HELP
  // =========================
  if (cmd == "help" || cmd == "?") {
    mostrarMenu();
  }
  
  // =========================
  // COMANDO SERVO (INTERACTIVO)
  // =========================
  else if (cmd == "servo") {
    Serial.println("Ingrese el angulo (0-180):");
    esperandoParametroServo = true;
  }
  
  // =========================
  // COMANDO SENSOR
  // =========================
  else if (cmd.startsWith("sensor ")) {
    int espacio = cmd.indexOf(' ');
    if (espacio != -1) {
      intervalo = cmd.substring(espacio + 1).toInt();
      
      if (intervalo < 100) {
        Serial.println("Error: Intervalo mínimo es 100 ms");
        mostrarPrompt();
        return;
      }
      
      leyendoSensor = true;
      tiempoAnterior = millis();
      Serial.print("Iniciando lectura cada ");
      Serial.print(intervalo);
      Serial.println(" ms");
      Serial.println("Presione 'x' para detener");
      Serial.println("---");
    } else {
      Serial.println("Uso: sensor <intervalo_ms>");
      Serial.println("  Ejemplo: sensor 1000");
      mostrarPrompt();
    }
  }
  
  // =========================
  // COMANDO WELCOME
  // =========================
  else if (cmd.startsWith("welcome ")) {
    int espacio = cmd.indexOf(' ');
    if (espacio != -1) {
      String mensaje = cmd.substring(espacio + 1);
      guardarMensajeEEPROM(mensaje);
      Serial.println("Mensaje de bienvenida guardado");
    } else {
      Serial.println("Uso: welcome <mensaje>");
      Serial.println("  Ejemplo: welcome Sistema Arduino v1.0");
    }
    mostrarPrompt();
  }
  
  // =========================
  // COMANDO DUMP (MEMORIA RAM)
  // =========================
  else if (cmd.startsWith("dump ")) {
    // Formato: dump <inicio> <fin>
    int primerEspacio = cmd.indexOf(' ');
    int segundoEspacio = cmd.indexOf(' ', primerEspacio + 1);
    
    if (primerEspacio != -1 && segundoEspacio != -1) {
      int inicio = cmd.substring(primerEspacio + 1, segundoEspacio).toInt();
      int fin = cmd.substring(segundoEspacio + 1).toInt();
      
      if (inicio >= 0 && fin > inicio && fin < RAMEND) {
        Serial.println("=== DUMP MEMORIA RAM ===");
        Serial.print("Rango: 0x");
        Serial.print(inicio, HEX);
        Serial.print(" - 0x");
        Serial.println(fin, HEX);
        Serial.println("--- DATOS BINARIOS ---");
        
        // Enviar datos binarios
        for (int i = inicio; i <= fin; i++) {
          byte valor = *((byte*)i);
          Serial.write(valor);
        }
        
        Serial.println("\n--- FIN DUMP ---");
      } else {
        Serial.println("Error: Rango invalido");
      }
    } else {
      Serial.println("Uso: dump <inicio> <fin>");
      Serial.println("  Ejemplo: dump 100 200");
    }
    mostrarPrompt();
  }
  
  // =========================
  // COMANDO CLEAR
  // =========================
  else if (cmd == "clear" || cmd == "cls") {
    for (int i = 0; i < 30; i++) {
      Serial.println();
    }
    mostrarMenu();
  }
  
  // =========================
  // COMANDO INVALIDO
  // =========================
  else {
    Serial.print("Comando no reconocido: '");
    Serial.print(cmd);
    Serial.println("'");
    Serial.println("Escriba 'help' para ver comandos disponibles");
    mostrarPrompt();
  }
}

// =========================
// FUNCIONES AUXILIARES
// =========================

void mostrarMenu() {
  Serial.println("   TERMINAL INTERACTIVA ARDUINO   ");
  Serial.println("Comandos disponibles:");
  Serial.println("  servo            - Controlar servomotor (interactivo)");
  Serial.println("  sensor <ms>      - Leer sensor con intervalo");
  Serial.println("  welcome <texto>  - Mensaje de bienvenida persistente");
  Serial.println("  dump <ini> <fin> - Dump binario de memoria RAM");
  Serial.println("  help             - Mostrar este menu");
  Serial.println("  clear            - Limpiar pantalla");
  Serial.println("-------------------------------------");
  mostrarPrompt();
}

void mostrarPrompt() {
  Serial.print("\n> ");
}

String leerMensajeEEPROM() {
  String mensaje = "";
  for (int i = 0; i < 100; i++) {
    char c = EEPROM.read(i);
    if (c == '\0' || c == 0xFF) break;
    mensaje += c;
  }
  return mensaje;
}

void guardarMensajeEEPROM(String mensaje) {
  // Borrar área de EEPROM
  for (int i = 0; i < 100; i++) {
    EEPROM.write(i, 0);
  }
  
  // Guardar mensaje
  for (unsigned int i = 0; i < mensaje.length() && i < 99; i++) {
    EEPROM.write(i, mensaje[i]);
  }
  EEPROM.write(mensaje.length(), '\0');
}
