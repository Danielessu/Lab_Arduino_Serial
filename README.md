# Laboratorio Arduino - Comunicación Serial e Interfaces Interactivas

## Integrantes

- Ana María Cordero Escobar  
- Laura Vanesa Reyes Martínez  
- Juan Esteban Ramírez Hermosa  
- Daniel Felipe Esquinas Suárez  

---

Enlace Tinkercad Parte I: https://www.tinkercad.com/things/2i2QEOBS9OP-copy-of-comunicacion-serial-1/editel?returnTo=https%3A%2F%2Fwww.tinkercad.com%2Fdashboard%2Fdesigns%2Fcircuits&sharecode=KHdzZ0fjAkI_won6Q6Me6vM6K2xVfXz53ND1M7SN4Oo

Enlace tinkercad Parte II: https://www.tinkercad.com/things/gbmYvzL3wgf-lab-comunicacion-serial-parte-ii-/editel?returnTo=https%3A%2F%2Fwww.tinkercad.com%2Fdashboard%2Fdesigns%2Fall&sharecode=Wm6dOgnRx-xJiCAQjYCT-j1X7XybP-IRnibArDtO5Bo

---
# Descripción del proyecto

Este laboratorio implementa un sistema de comunicación serial utilizando Arduino Uno. El proyecto se divide en varias partes:

- Implementación de una terminal interactiva mediante comunicación serial.
- Control de actuadores y lectura continua de sensores mediante comandos.
- Almacenamiento persistente de mensajes usando EEPROM.
- Comunicación serial entre dos Arduino Uno.
- Interfaz de usuario usando teclado matricial y pantalla LCD I2C.
- Implementación de un sistema de escritura tipo T9.
- Automatización de compilación mediante GitHub Actions.

---

# Objetivos

- Implementar una terminal interactiva estilo Bash/Powershell.
- Utilizar comunicación serial UART entre dispositivos.
- Controlar actuadores desde comandos interactivos.
- Implementar lectura periódica de sensores.
- Utilizar memoria EEPROM para almacenamiento persistente.
- Diseñar una interfaz de usuario embebida.
- Automatizar pruebas de compilación con CI/CD.

---

# Tecnologías utilizadas

- Arduino Uno
- Arduino IDE
- Tinkercad
- GitHub
- GitHub Actions
- EEPROM
- Comunicación Serial UART
- LCD I2C 16x2
- Keypad 4x4
- Servomotor
- Sensor analógico

---

# Librerías utilizadas

```cpp
Servo.h
EEPROM.h
Wire.h
LiquidCrystal_I2C.h
Keypad.h
```

---

# Estructura del repositorio

```text
Lab_Arduino_Serial/
│
├── README.md
│
├── Parte_I/
│   └── Parte_I.ino
│
├── Parte_II/
│   ├── Servo/
│   │   └── Servo.ino
│   │
│   └── Interfaz/
│       └── Interfaz.ino
│
└── .github/
    └── workflows/
        └── arduino.yml
```

---

# Parte I - Terminal interactiva

Se implementó una terminal serial interactiva en Arduino Uno capaz de interpretar comandos enviados mediante UART.

## Funcionalidades implementadas

### Control de servomotor

Permite controlar el ángulo del servomotor mediante comandos seriales.

### Lectura continua de sensor

Permite leer continuamente un sensor analógico utilizando un intervalo configurable.

### Detención de lectura

La lectura continua puede detenerse mediante un comando específico.

### Almacenamiento en EEPROM

Se implementó almacenamiento persistente de mensajes utilizando EEPROM.

### Timestamps

Cada lectura del sensor incluye una marca temporal utilizando `millis()`.

---

# Comandos implementados

| Comando | Función |
|---|---|
| `SERVO <angulo>` | Mueve el servomotor |
| `SENSOR <ms>` | Inicia lectura continua |
| `STOP` | Detiene lectura |
| `WELCOME` | Muestra mensaje EEPROM |
| `SETMSG <mensaje>` | Guarda mensaje EEPROM |
| `HELP` | Muestra ayuda |

---

# Parte II - Comunicación entre dos Arduino

Se interconectaron dos Arduino Uno mediante comunicación serial UART.

## Arduino Servo

Responsable de:
- Control del servomotor
- Lectura del sensor
- Gestión de EEPROM
- Procesamiento de comandos

## Arduino Interfaz

Responsable de:
- Lectura del teclado matricial
- Visualización en LCD I2C
- Interfaz de usuario
- Sistema T9

---

# Sistema T9

Se implementó un sistema de escritura inspirado en teléfonos móviles antiguos para permitir el ingreso de texto usando un teclado matricial limitado.

## Distribución T9

| Tecla | Letras |
|---|---|
| 2 | ABC |
| 3 | DEF |
| 4 | GHI |
| 5 | JKL |
| 6 | MNO |
| 7 | PQRS |
| 8 | TUV |
| 9 | WXYZ |
| 0 | Espacio |
| * | Borrar carácter |
| D | Cancelar |
| # | Enviar / Guardar |
---

# Parte III - Comunicación a 20 metros

La comunicación UART TTL directa no es recomendable para distancias de 20 metros debido a:

- Sensibilidad al ruido
- Caída de voltaje
- Pérdida de integridad de señal

## Solución recomendada

Utilizar comunicación diferencial RS-485 mediante módulos MAX485 y cable trenzado AWG24 para garantizar una transmisión confiable a larga distancia.

---

# GitHub Actions

Se implementó un workflow de GitHub Actions para compilar automáticamente todos los sketches `.ino` en cada push realizado al repositorio.

## Funcionalidades del workflow

- Instalación automática de Arduino CLI
- Instalación del core AVR
- Instalación automática de librerías
- Compilación automática de todos los sketches

---
