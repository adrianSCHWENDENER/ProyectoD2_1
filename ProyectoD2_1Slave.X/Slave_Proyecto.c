/*
 * File:   Slave_Proyecto.c
 * Author: schwe
 *
 * Created on 22 de agosto de 2023, 01:02 PM
 */

// CONFIG1
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//Librerías
#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include "I2C.h"
#include "ADC_int.h"

//Definición de variables
#define _XTAL_FREQ 4000000
uint8_t z;
uint8_t ADC;
int pwm;

//Prototipos
void setup(void);
void main(void);
int mapear (int valor, int min, int max, int nmin, int nmax);

//Setup General
void setup(void){
    //Oscilador
    OSCCON = 0B01100001;                                    //Oscilador a 4Mhz
    
    //Interrupciones
    INTCON = 0B11000000;                                    //Int globales, PIE1 activadas
    PIE1 = 0B01000110;                                      //Int ADC, CCPI1 y TMR2 match activadas
    PIR1 = 0B00000000;
    OPTION_REG = 0;

    //ADC
    setup_ADC(0);

    //I2C
    I2C_Slave_Init(0x20);
    
    //PWM
    TRISCbits.TRISC2 = 1;
    PR2 = 250;                                              //Periodo de 4ms
    CCP1CON = 0B00001100;                                   //PWM mode, P1A, B, C y D normales, LSbs en 00
    CCPR1L = 0B00000000;                                    //Duty cicle inicial en 0
    
    //TIMER2
    PIR1bits.TMR2IF = 0;
    T2CON = 0B00000111;                                     //TMR2 activado, prescaler 16
    while (!PIR1bits.TMR2IF){                               //Esperar a que TMR2 haga overflow
    ;
    }
    TRISCbits.TRISC2 = 0;
    
    //Entradas y salidas
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    
    //Valores iniciales de variables y puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
    
    return;
}

//Interrupcion
void __interrupt() isr(void) {
    if (PIR1bits.ADIF){
        ADC = read_ADC();
    }
    
    if (SSPIF == 1){
        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW){
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
        }
        else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = ADC;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
    }
        
    if (PIR1bits.TMR2IF){
        PIR1bits.TMR2IF = 0;
    }

    PIR1bits.ADIF = 0;                                      //Limpia la bandera de interrupción
    SSPIF = 0;
    return;
}

//Loop
void main(void) {
    setup();
    while(1){
        if (ADCON0bits.GO == 0){
            __delay_ms(5);
            ADCON0bits.GO = 1;
        }
        
        if (ADC > 200){
            pwm = 628;              //mapear(ADC, 0, 255, 124, 628);
            CCPR1L = (unsigned char)(pwm>>2);        
            CCP1CONbits.DC1B0 = (pwm & 0B01);
            CCP1CONbits.DC1B1 = (pwm & 0B10)>>1;
        }
        else{
            pwm = 124;
            CCPR1L = (unsigned char)(pwm>>2);        
            CCP1CONbits.DC1B0 = (pwm & 0B01);
            CCP1CONbits.DC1B1 = (pwm & 0B10)>>1;
        }
    }
}

//Funciones
int mapear (int valor, int min, int max, int nmin, int nmax){
    int nvalor = nmin + (valor - min) * (long)(nmax - nmin) / (max - min);
    return nvalor;
}