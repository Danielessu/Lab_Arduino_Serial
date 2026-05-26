#include <Servo.h>
#include <EEPROM.h>

Servo servoMotor;

const int PIN_SERVO  = 5;
const int PIN_SENSOR = A0;

#define EEPROM_ADDR 0
#define MSG_MAX 24

bool leyendoSensor = false;

unsigned long intervaloMS = 500;
unsigned long tiempoAnterior = 0;

int ultimoAngulo = 90;

char buffer[50];
byte indice = 0;

// ======================================================
// EEPROM
// ======================================================

String leerMensajeEEPROM() {

  String msg = "";

  for (int i = 0; i < MSG_MAX; i++) {

    byte b = EEPROM.read(EEPROM_ADDR + i);

    // FIN DEL MENSAJE
    if (b == 0xFF || b == 0x00) {
      break;
    }

    // SOLO CARACTERES IMPRIMIBLES
    if (b >= 32 && b <= 126) {

      msg += (char)b;
    }
  }

  // MENSAJE POR DEFECTO
  if (msg.length() == 0) {

    msg = "Hola";
  }

  return msg;
}

void guardarMensajeEEPROM(String msg) {

  int len = min((int)msg.length(), MSG_MAX - 1);

  for (int i = 0; i < len; i++) {

    EEPROM.write(EEPROM_ADDR + i, msg[i]);
  }

  EEPROM.write(EEPROM_ADDR + len, 0);
}

// ======================================================
// LEER SERIAL
// ======================================================

bool leerLinea(char* buf, byte& idx, byte maxLen) {

  while (Serial.available()) {

    char c = Serial.read();

    if (c == '\r') continue;

    if (c == '\n') {

      buf[idx] = '\0';

      idx = 0;

      return true;
    }

    if (idx < maxLen - 1) {

      buf[idx++] = c;
    }
  }

  return false;
}

// ======================================================
// COMANDOS
// ======================================================

void ejecutarComando(String cmd) {

  cmd.trim();

  Serial.print("CMD -> ");
  Serial.println(cmd);

  // ==================================================
  // SERVO
  // ==================================================

  if (cmd.startsWith("SERVO ")) {

    int angulo = cmd.substring(6).toInt();

    if (angulo < 0 || angulo > 180) {

      Serial.println("Error: rango 0-180");
      return;
    }

    ultimoAngulo = angulo;

    servoMotor.write(angulo);

    Serial.print("Servo -> ");
    Serial.print(angulo);
    Serial.println(" grados");
  }

  // ==================================================
  // SENSOR
  // ==================================================

  else if (cmd.startsWith("SENSOR ")) {

    intervaloMS = cmd.substring(7).toInt();

    if (intervaloMS < 50) {
      intervaloMS = 50;
    }

    leyendoSensor = true;

    tiempoAnterior = millis();

    Serial.print("Sensor ON -> ");
    Serial.print(intervaloMS);
    Serial.println(" ms");
  }

  // ==================================================
  // STOP
  // ==================================================

  else if (cmd == "STOP") {

    leyendoSensor = false;

    Serial.println("Sensor OFF");
  }

  // ==================================================
  // WELCOME
  // ==================================================

  else if (cmd == "WELCOME") {

    Serial.print("EEPROM -> ");
    Serial.println(leerMensajeEEPROM());
  }

  // ==================================================
  // SETMSG
  // ==================================================

  else if (cmd.startsWith("SETMSG ")) {

    String nuevo = cmd.substring(7);

    guardarMensajeEEPROM(nuevo);

    Serial.println("EEPROM OK");
  }

  // ==================================================
  // HELP
  // ==================================================

  else if (cmd == "HELP") {

    Serial.println("SERVO <0-180>");
    Serial.println("SENSOR <ms>");
    Serial.println("STOP");
    Serial.println("WELCOME");
    Serial.println("SETMSG <msg>");
  }

  else {

    Serial.println("CMD ERROR");
  }
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(9600);

  // Esperar estabilizacion serial
  delay(500);

  servoMotor.attach(PIN_SERVO);

  servoMotor.write(90);

  pinMode(PIN_SENSOR, INPUT);

  Serial.print("Mensaje EEPROM: ");
  Serial.println(leerMensajeEEPROM());

  Serial.println("Sistema listo");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  // ==================================================
  // COMANDOS
  // ==================================================

  if (leerLinea(buffer, indice, sizeof(buffer))) {

    ejecutarComando(String(buffer));
  }

  // ==================================================
  // SENSOR CONTINUO
  // ==================================================

  if (leyendoSensor) {

    unsigned long ahora = millis();

    if (ahora - tiempoAnterior >= intervaloMS) {

      tiempoAnterior = ahora;

      int valor = analogRead(PIN_SENSOR);

      Serial.print("T:");
      Serial.print(ahora);

      Serial.print("  V:");
      Serial.println(valor);
    }
  }
}
//test workflow 2
