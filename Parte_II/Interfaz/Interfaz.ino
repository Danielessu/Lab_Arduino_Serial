#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte FILAS = 4;
const byte COLS  = 4;

char teclas[FILAS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pFilas[FILAS] = {2,3,4,5};
byte pCols [COLS]  = {6,7,8,9};

Keypad keypad = Keypad(
  makeKeymap(teclas),
  pFilas,
  pCols,
  FILAS,
  COLS
);

enum Estado {
  MENU,
  SERVO_MODE,
  SENSOR_MODE,
  MSG_MODE
};

Estado estado = MENU;

String buffer = "";

char rxBuf[50];
byte rxIdx = 0;

// ======================================================
// LCD
// ======================================================

void lcdLinea(byte fila, String txt) {

  lcd.setCursor(0, fila);

  while (txt.length() < 16) {
    txt += " ";
  }

  lcd.print(txt.substring(0,16));
}

void mostrarMenu() {

  lcd.clear();

  lcdLinea(0, "A Servo B Sens");
  lcdLinea(1, "C Stop  D Msg");

  estado = MENU;

  buffer = "";
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcdLinea(0, "Terminal");
  lcdLinea(1, "Arduino");

  delay(1500);

  mostrarMenu();
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  char t = keypad.getKey();

  // ==================================================
  // TECLAS
  // ==================================================

  if (t) {

    // ==============================================
    // MENU
    // ==============================================

    if (estado == MENU) {

      // SERVO
      if (t == 'A') {

        estado = SERVO_MODE;

        buffer = "";

        lcd.clear();

        lcdLinea(0, "Servo 0-180");
        lcdLinea(1, "# enviar");
      }

      // SENSOR
      else if (t == 'B') {

        estado = SENSOR_MODE;

        buffer = "";

        lcd.clear();

        lcdLinea(0, "Intervalo ms");
        lcdLinea(1, "# iniciar");
      }

      // STOP
      else if (t == 'C') {

        Serial.println("STOP");

        lcd.clear();

        lcdLinea(0, "Sensor OFF");

        delay(500);

        mostrarMenu();
      }

      // MENSAJES
      else if (t == 'D') {

        estado = MSG_MODE;

        buffer = "";

        lcd.clear();

        lcdLinea(0, "Mensaje T9");
        lcdLinea(1, "2=ABC #=OK");
      }
    }

    // ==============================================
    // SERVO
    // ==============================================

    else if (estado == SERVO_MODE) {

      if (t >= '0' && t <= '9') {

        if (buffer.length() < 3) {

          buffer += t;

          lcdLinea(1, buffer + " grados");
        }
      }

      else if (t == '*') {

        buffer = "";

        lcdLinea(1, "");
      }

      else if (t == '#') {

        int ang = buffer.toInt();

        if (ang < 0 || ang > 180) {

          lcdLinea(0, "Max 180");
          delay(700);
        }

        else {

          Serial.print("SERVO ");
          Serial.println(buffer);

          lcd.clear();

          lcdLinea(0, "Servo -> " + buffer);

          delay(500);
        }

        mostrarMenu();
      }
    }

    // ==============================================
    // SENSOR
    // ==============================================

    else if (estado == SENSOR_MODE) {

      if (t >= '0' && t <= '9') {

        if (buffer.length() < 5) {

          buffer += t;

          lcdLinea(1, buffer + " ms");
        }
      }

      else if (t == '*') {

        buffer = "";

        lcdLinea(1, "");
      }

      else if (t == '#') {

        Serial.print("SENSOR ");
        Serial.println(buffer);

        lcd.clear();

        lcdLinea(0, "Sens ON");
        lcdLinea(1, buffer + " ms");

        delay(500);

        mostrarMenu();
      }
    }

    // ==============================================
    // MSG MODE T9
    // ==============================================

    else if (estado == MSG_MODE) {

      static unsigned long ultimaPulsacion = 0;
      static char ultimaTecla = '\0';
      static int indiceLetra = 0;

      String letras = "";

      if (t == '2') letras = "ABC";
      else if (t == '3') letras = "DEF";
      else if (t == '4') letras = "GHI";
      else if (t == '5') letras = "JKL";
      else if (t == '6') letras = "MNO";
      else if (t == '7') letras = "PQRS";
      else if (t == '8') letras = "TUV";
      else if (t == '9') letras = "WXYZ";
      else if (t == '0') letras = " ";

      // ==========================================
      // LETRAS
      // ==========================================

      if (letras.length() > 0) {

        unsigned long ahora = millis();

        if (t == ultimaTecla && (ahora - ultimaPulsacion < 1000)) {

          indiceLetra++;

          if (indiceLetra >= letras.length()) {
            indiceLetra = 0;
          }

          if (buffer.length() > 0) {
            buffer.remove(buffer.length() - 1);
          }

          buffer += letras[indiceLetra];
        }

        else {

          indiceLetra = 0;

          buffer += letras[indiceLetra];
        }

        ultimaTecla = t;

        ultimaPulsacion = ahora;

        lcdLinea(1, buffer);
      }

      // ==========================================
      // BORRAR
      // ==========================================

      else if (t == '*') {

        if (buffer.length() > 0) {

          buffer.remove(buffer.length() - 1);

          lcdLinea(1, buffer);
        }
      }

      // ==========================================
      // GUARDAR
      // ==========================================

      else if (t == '#') {

        Serial.print("SETMSG ");
        Serial.println(buffer);

        lcd.clear();

        lcdLinea(0, "EEPROM OK");
        lcdLinea(1, buffer);

        delay(800);

        mostrarMenu();
      }

      // ==========================================
      // SALIR
      // ==========================================

      else if (t == 'D') {

        mostrarMenu();
      }
    }
  }

  // ==================================================
  // RESPUESTAS SERIAL
  // ==================================================

  while (Serial.available()) {

    char c = Serial.read();

    if (c == '\r') continue;

    if (c == '\n') {

      rxBuf[rxIdx] = '\0';

      String resp = String(rxBuf);

      // SENSOR
      if (resp.startsWith("T:")) {

        int pos = resp.indexOf("V:");

        String valor = resp.substring(pos + 2);

        lcdLinea(0, "Sensor activo");
        lcdLinea(1, "Valor: " + valor);
      }

      else {

        lcd.clear();

        lcdLinea(0, resp.substring(0,16));

        if (resp.length() > 16) {
          lcdLinea(1, resp.substring(16,32));
        }

        delay(500);

        mostrarMenu();
      }

      rxIdx = 0;
    }

    else {

      if (rxIdx < sizeof(rxBuf)-1) {

        rxBuf[rxIdx++] = c;
      }
    }
  }
}
