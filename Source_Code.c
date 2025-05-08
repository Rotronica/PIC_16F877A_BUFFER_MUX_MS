/*
 * File:   Source_Code.c
 * Author: Rodrigo C.C.
 *
 * Created on May 8, 2025, 2:48 PM
 */

#include <xc.h>
#include "fusibles.h"
#define _XTAL_FREQ 4000000
#include <stdbool.h>

#define MUX_TIME        5       //Tiempo de multiplexacion
#define NUM_DISPLAY     4       //Se utiliza 4 display
#define DISPLAY_MASK    0x0F    // Bits 0-3 para displays(Para la mascara de seguridad)

#define PINES_MUX           PORTA
#define PUERTO_VISUALIZADOR PORTD

uint32_t milisegundos = 0;  //Variable que acumula los milisegundos
// Tabla de caracteres (cátodo común)
static const uint8_t DATOS[] = {
    // Números 0-9
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    // Letras A-Z
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x30, 0x1E,
    0x76, 0x38, 0x37, 0x54, 0x3F, 0x73, 0x67, 0x50, 0x6D, 0x78,
    0x3E, 0x3E, 0x2A, 0x76, 0x6E, 0x5B,
    // Símbolos especiales
    0x40, // Guión (-)
    0x80, // Punto decimal (.)
    0x63, // Grados (°)
    0x00  // Espacio (apagado) 
};
//Indices simbolicos
typedef enum{
    CHAR_H = 17,
    CHAR_O = 24,
    CHAR_L = 21,
    CHAR_A = 10,
    CHAR_I = 18,
    CHAR_M = 22,
    CHAR_GUION = 36,
    CHAR_OFF = 39
}CARACTER;
CARACTER display_buffer[NUM_DISPLAY] = {CHAR_GUION,CHAR_GUION,CHAR_GUION,CHAR_GUION};//Se inicializa con el mensaje hola
uint8_t display_state = 0;  //Estado de display

/* 
 * ======================================
 *      Prototipo de funciones
 * ======================================
 */
void configurar_registros(void);
void configurar_tmr0(void);
void visualizar_display(void);
uint32_t millis(void);
void mostrar_mensajes(const uint8_t *mensaje, uint8_t longitud);


void configurar_registros(void){
    TRISA = 0X00;
    TRISD = 0X00;
    PORTA = 0X00;
    PORTD = 0X00;
}
void configurar_tmr0(void){
    //Configuracion par utilizar TMR0 modo Temporizador
    OPTION_REGbits.PS = 0b010;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.T0CS = 0;
    //Activacion de la interrupcion TMR0
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    //Carga del registro TMR0 par una temporizacion de 1ms
    TMR0 = 131;
}
void __interrupt() INT_TMR0(void){
    if(INTCONbits.T0IF == 1){
        INTCONbits.T0IF = 0;    //Limpia la bandera del int. TMR0
        TMR0 = 131;
        milisegundos++;
    }
}
uint32_t millis(void){
    uint32_t m;
    INTCONbits.GIE = 0;
    m = milisegundos;
    INTCONbits.GIE = 1;
    return m;
}
//Con esta funcion se realiza la visualizacion y la multiplexacion
void visualizar_display(void){
    static uint32_t last_mux = 0;
    uint32_t now = millis();
    //Multiplexacion cada 5ms(200Hz de tasa de refresco total)
    if((now - last_mux) >= MUX_TIME){
        last_mux = now;
        //Apagar todos los display
        PINES_MUX &= ~DISPLAY_MASK;  // Solo afecta bits de displays
        //Mostrar el caracter actual
        PUERTO_VISUALIZADOR = DATOS[display_buffer[display_state]];//Aqui dice que primero ingrese al array display_buffer tomara un numero y con ese numero tomara el valor del array DATOS 
        //Activar el display correspondiente
        PINES_MUX |= 1 << display_state; // Enciende el bit correspondiente para cada display con seguridad implementando un or como mascara esto evitara el ghosting
        //Avanzar al siguiente display
        display_state = (display_state + 1) % NUM_DISPLAY; //Con esta sentencia aumente display_state 1,2,3,4 a la vez controla el limite que es la cantidad de display utilizados cuando llega a 4 automaticamente lo lleva al primer displey   
    }
}

void mostrar_mensajes(const uint8_t *mensaje, uint8_t longitud){
    for(uint8_t i = 0; i < NUM_DISPLAY; i++){
        display_buffer[i] = (i < longitud) ? mensaje[i] : CHAR_OFF;
    }
}
void main(void) {
    uint32_t last_change = 0;
    bool mostrar_dos_ms = 0;
    const uint8_t hola[] = {CHAR_H,CHAR_O,CHAR_L,CHAR_A};
    const uint8_t rodo[] = {CHAR_L,CHAR_I,CHAR_A,CHAR_M};
    configurar_registros();
    configurar_tmr0();
    while(true){
        visualizar_display();
        uint32_t now = millis();
        if((now - last_change) >= 1000){
            last_change = now;
            mostrar_dos_ms = !mostrar_dos_ms;
            mostrar_mensajes((mostrar_dos_ms == true)? hola:rodo, 4);
        }
    }
}
