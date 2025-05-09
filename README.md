<h1>Multiplexación con buffer para caracteres con PIC16F877A</h1>
<p>
  El siguiente código realiza la multiplexación de cuatro display el cual puede mostrar diferentes mensajes, el código proporcionado visualiza el mensaja "HOLA LIAM"
</p>
<h2>Función visualizar_display()</h2>
<p>
  La forma de como realiza la multiplexación esta implementado en la función void visualizar_display(void). El tiempo de multiplexación es de 5 milisegundos, no se hace uso de 
  la sentencia __delay_ms() por que en el código trata de que la ejecución no se interrumpa y una manera de no detener la ejecucion del codigo es hacer uso de una función millis().
</p>
<p>
  Entonces para implementar el tiempo para cada display se hace uso de:
  
```c
  uint32_t now = millis();
    // Multiplexación cada 5ms (200Hz de tasa de refresco total)
    if ((now - last_mux) >= MUX_TIME) {
        last_mux = now;
```
  Se crea una variable con un tipo de dato con 32 bits sin signo el cual se iguala a la funcion millis(): uint32_t now = millis();
  la función millis() se incrementa cada 1ms entonces para obtener un tiempo de 5ms la variable MUX_TIME = 5. Si se repite 5 veces 1ms se optiene 5ms, lo que hace if es controlar 
  la sentencia (now - last_mux) si es mayor o igual a MUX_TIME entra al if lo primero que hace es guardar la variable now en la variable last_mux.
</p>
<h3>Forma de apagar todos los pines que activan los display</h3>
<p>
  La sentencia:

  ```c
  // Apagar todos los displays
        PINES_MUX &= ~DISPLAY_MASK;
  ```
  Lo que hace es apagar los 4 pines que hacen control de la activación de cada display de forma segura implementando una mascara donde  DISPLAY_MASK = 0x0F y PINES_MUX = PORTA
  entonces:
  <ol>
    <li> ~DISPLAY_MASK: Invierte los bits de la máscara (si DISPLAY_MASK = 0x0F → ~0x0F = 0xF0)</li>
    <li> &=: Realiza un AND bit a bit con el valor actual del puerto, preservando los otros bits no relacionados</li>
  </ol>
  Entonces inicialmente PINES_MUX tiene un valor de 0x00 y DISPLAY_MASK = 0x0F la sentencia tiene la siguiente forma  PINES_MUX =  PINES_MUX & ~DISPLAY_MASK,
  remplazando valores: PINES_MUX = 0b00000000 & 0b11110000 resultado aplicando and PINES_MUX = 0b00000000, esto hace que los pines encargados de la activación 
  de los display se apaguen de forma segura sin afectar a los demas pines si esque se hace uso para otras funcionalidades.
</p>
<h3>Sentencia que se encarga de tomar los datos para cada letra en cada display </h3>
<p>
  La sentencia:

  ```c
  // Mostrar el carácter actual
  PUERTO_VISUALIZADOR = DATOS[display_buffer[display_state]];
  ```
  Se encarga de tomar el caracter correspondiente y visualizarlo por el PORTD que esta inicializado con la variable PUERTO_VISUALIZADOR. La forma de tomar los caracteres tiene estos pasos:
  <ol>
    <li> Lo primero que hace es acceder al buffer de caracteres que contiene el mensaja: display_buffer[display_state] para tomar el dato de este array depende del estado de la 
         variable display_state que tambien esta variable controla la activacion de cada display.</li>
    <li> Una vez con el dato del buffer display_buffer[ ] este será el dato para tomar el dato del array DATOS[ ].</li>
  </ol>
</p>
<h3> Sentencia que activa cada display</h3>
<p>
  La forma de activar cada display de forma segura es implentando una mascara:
  
  ```c
  // Activar el display correspondiente
      MUX_PORT |= (1 << display_state);
  ```
Esto es equivalente a:
```c
 PINES_MUX = MUX_PORT | (1 << display_state);
  ```
Inicialmente PINES_MUX sera igual a 0x00 y la instrucción (1 << display_state) desplaza el bit 1 a la izquierda display_state posiciones
</p>
<h3>Sentencia que se encarga de avanzar la activacion de los display</h3>
<p>
  Esta sentencia se actualiza de forma automatica a medida que avanza cuando llega a cuatro la variable display_state pasa a valer nuevamente uno esto gracias a la operación residuo
  
  ```c
  // Avanzar al siguiente display
        display_state = (display_state + 1) % NUM_DISPLAYS;
  ```
</p>
<h2> Código completo funcion visualizar_display()</h2>

```c
  void visualizar_display() {
    static uint32_t last_mux = 0;
    uint32_t now = millis();
    
    // Multiplexación cada 5ms (200Hz de tasa de refresco total)
    if ((now - last_mux) >= MUX_TIME) {
        last_mux = now;
        
        // Apagar todos los displays
        PINES_MUX &= ~DISPLAY_MASK;
        
        // Mostrar el carácter actual
        PUERTO_VISUALIZADOR = DATOS[display_buffer[display_state]];
        
        // Activar el display correspondiente
        PINES_MUX |= (1 << display_state);
        
        // Avanzar al siguiente display
        display_state = (display_state + 1) % NUM_DISPLAYS;
    }
}

