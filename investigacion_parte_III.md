# Comunicación Arduino a 20 m — AWG 24

**Integrantes:** Ana María Cordero, Laura Vanesa Reyes, Juan Esteban Ramírez, Daniel Esquinas

---

# 1. Problema: UART TTL directo a 20 m

- UART TTL usa señal de extremo único (*single-ended*): un conductor contra GND.
- Límite práctico: ~3 m sin repetidores ni buffers → más allá, errores de datos frecuentes.
- A 20 m la señal se degrada por:
  - Resistencia del cable → caída de tensión.
  - Capacitancia distribuida → redondeo de flancos (*bit distortion*).
  - Ruido electromagnético externo acoplado al conductor.

## Conclusión

UART TTL a 20 m con AWG 24 **NO es viable** para comunicación confiable.

---

# 2. Solución: RS-485 con módulos MAX485

## ¿Por qué RS-485?

- Señalización diferencial: par A/B con señales complementarias → inmune al ruido de modo común.
- Alcance: hasta 1200 m a 9600 bps (a 20 m hay margen amplio).
- Estándar industrial: I2C/SPI son para distancias cortas; RS-485 es para distancias > 10–15 m.
- Comunicación semidúplex (TX o RX, no simultáneo) → se controla con pin DE/RE.

## Chip MAX485 (características clave)

- Alimentación: +5 V, corriente 120–500 µA.
- Velocidad máxima: 2.5 Mbps.
- Pines de control:
  - DE (*Driver Enable*)
  - RE (*Receiver Enable*)
- Fail-safe: salida garantizada en HIGH si entrada está en circuito abierto.
- Protección:
  - Limitación de cortocircuito.
  - Apagado térmico.

---

# 3. ¿Es válido el cable AWG 24 para RS-485?

- Sí, pero debe usarse como par trenzado (*twisted pair*), no conductores paralelos sueltos.
- Especificación RS-422/485 recomienda exactamente AWG 24 par trenzado:
  - Capacitancia: ≤ 16 pF/pie (≤ 52.4 pF/m).
  - Impedancia característica: 100–130 Ω.
  - Capacitancia entre conductor y blindaje: < 200 pF/m.

## Alternativa económica

Cable Ethernet CAT-5/CAT-6:

- AWG 24.
- 4 pares trenzados.
- ~15 pF/pie.
- 100 Ω.

→ Cumple requisitos.

## Recomendación

Blindaje recomendado para entornos ruidosos:

- Motores.
- Variadores de frecuencia.
- Equipos industriales.

---

# 4. Requisitos de implementación

## Hardware necesario

- 2× módulo MAX485 (uno por Arduino) — costo ~$1–2 USD c/u.
- Cable AWG 24 par trenzado (o CAT-5/6) de 20 m.
- 2× resistencia de terminación 120 Ω (una en cada extremo del bus).

---

# Resistencias de terminación — ¿Por qué son obligatorias?

- Sin terminación → ondas estacionarias en el cable → fallas de comunicación impredecibles.
- Valor: 120 Ω (iguala la impedancia característica del cable).
- Ubicación: en los dos extremos del bus, entre líneas A y B.

---

# Esquema de conexión

## Arduino 1

- TX → DI del MAX485 #1.
- RO del MAX485 #1 → RX.

## Arduino 2

- TX → DI del MAX485 #2.
- RO del MAX485 #2 → RX.

## Bus diferencial

- A del MAX485 #1 ══ A del MAX485 #2.
- B del MAX485 #1 ══ B del MAX485 #2.

## GND común

- GND #1 ══ GND #2.

Referencia de modo común requerida para confiabilidad.

## Control DE/RE

- Pin digital Arduino → DE/RE.
- HIGH → transmitir.
- LOW → recibir.

---

# 5. Baud rate recomendado

- A 20 m con AWG 24, cualquier velocidad estándar es segura.
- A < 90 Kbps la distancia máxima está limitada a 1200 m (resistencia DC del cable).
- A > 90 Kbps:
  - La distancia máxima disminuye con la velocidad.
  - Relación inversa velocidad/distancia.

## Recomendación para este proyecto

**9600 bps**:

- Máxima confiabilidad.
- Sin problema de distancia.

---

# 6. Resumen comparativo

| Tecnología | Resultado |
|---|---|
| UART TTL directo a 20 m | No viable — degradación, ruido, sin estándar para esta distancia |
| RS-485 con MAX485 + AWG 24 trenzado | Completamente viable — estándar industrial, económico y simple |

---

# Modificaciones necesarias

- 2× MAX485.
- Cable par trenzado.
- 2× resistencias 120 Ω.
- Control DE/RE en código.

---

# Escalabilidad

El mismo esquema funciona hasta ~1200 m si fuera necesario.

---

# Fuentes

- CIRCUITSTATE Electronics
- Maxim AN-292
- Advantech RS-485 Cable Selection
- CircuitDigest
