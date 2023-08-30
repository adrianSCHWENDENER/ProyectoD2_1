/*
 * File:   Principal_Proyecto.c
 * Author: schwe
 *
 * Created on 19 de agosto de 2023, 10:07 PM
 */

// CONFIG1
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
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
#include "LCD.h"
#include "mpu6050.h"

//Definición de variables
#define _XTAL_FREQ 8000000
char buffer[6];                                            //Variable para mandar a LCD
char buffer2[6];
float Ax, Ay;                                              //Variables para recibir valores
int AX, AY;
int Distance;
int LDR;

//Prototipos
void setup(void);
void main(void);

//Setup General
void setup(void){
    //Oscilador
    OSCCON = 0B01110001;                                    //Oscilador a 8Mhz
    
    //I2C
    I2C_Master_Init(100000);
    
    //Entradas y salidas
    ANSEL = 0;                                              //Ningún puerto analógico
    ANSELH = 0;
    TRISA = 0;                                              //Puertos como salidas
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

//Loop
void main(void) {
    setup();
    
    MPU6050_Init();                                         //Iniciar MPU

    Lcd_Init();                                             //Iniciar LCD
    Lcd_Clear();                                            //Limpiar LCD
    Lcd_Set_Cursor(1, 2);                                   //Colocar títulos en LCD
    Lcd_Write_String("Ax");
    Lcd_Set_Cursor(1, 7);
    Lcd_Write_String("Ay");
    Lcd_Set_Cursor(1, 13);
    Lcd_Write_String("Dist");
    __delay_ms(10);
    while(1){
        PORTAbits.RA0 = !PORTAbits.RA0;                     //LED indicador de loop
        Ax = MPU6050_Read_Ax();                             //Recibir Ax
        Ay = MPU6050_Read_Ay();                             //Recibir AY
        __delay_ms(20);
        
        AX = Ax*100;
        AY = Ay*100;
        
        I2C_Master_Start();                                 //Iniciar comunicación I2C
        I2C_Master_Write(0xC1);                             //Recibir datos de 0xC0
        Distance = I2C_Master_Read(0);                      //Guardar valor en Distance
        I2C_Master_Stop();                                  //Finalizar comuniación
        __delay_ms(20);

        I2C_Master_Start();                                 //Iniciar comunicación I2C
        I2C_Master_Write(0x21);                             //Recibir datos de 0x20
        LDR = I2C_Master_Read(0);                           //Guardar valor en LDR
        I2C_Master_Stop();                                  //Finalizar comuniación
        __delay_ms(20);
        
        I2C_Master_Start();                                 //Iniciar comunicación I2C
        I2C_Master_Write(0x60);                             //Mandar datos a 0x60
        I2C_Master_Write(AX);                               //Mandar AX
        I2C_Master_Write(AY);                               //Mandar AY
        I2C_Master_Write(Distance);                         //Mandar Distance
        I2C_Master_Stop();                                  //Finalizar comuniación
        __delay_ms(20);
                
        if(AY < -40){                                       //Si giró a -AY
            PORTDbits.RD0 = 1;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
        }
        else if(AY > 40){                                   //Si giró a AY
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 1;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
        }
        else if(AX > 48){                                   //Si giró a AX
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 1;
            PORTDbits.RD3 = 0;
        }
        else if(AX < -40){                                  //Si giró a -AX
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 1;
        }
        else{                                               //Si esta quieto
            PORTDbits.RD0 = 0;
            PORTDbits.RD1 = 0;
            PORTDbits.RD2 = 0;
            PORTDbits.RD3 = 0;
        }
        
        Lcd_Set_Cursor(2, 1);
        sprintf(buffer, "%d", AX);                          //Pasar a char
        if(AX > 0 && AX < 10){                              //Si es positivo menor a 10
            buffer2[3] = buffer[0];                         //Ubicar detras de "0.0"
            buffer2[2] = '0';
        }
        else if(AX > 0 && AX < 100){                        //Si es positivo menor a 100
            buffer2[3] = buffer[1];                         //Ubicar detras de "0."
            buffer2[2] = buffer[0];
        }
        else if(AX < 0 && AX > -10){                        //Si es negativo mayor a -10
            buffer2[3] = buffer[1];                         //Ubicar detras de "-0.0"
            buffer2[2] = '0';
            Lcd_Write_Char('-');
            Lcd_Set_Cursor(2, 2);
        }
        else if(AX < 0 && AX > -100){                       //Si es negativo mayor a -100
            buffer2[3] = buffer[2];                         //Ubicar detras de "-0."
            buffer2[2] = buffer[1];
            Lcd_Write_Char('-');
            Lcd_Set_Cursor(2, 2);
        }
        else{                                               //Si no se cumple
            buffer2[2] = '0';                               //Mostrar "0.00"
            buffer2[3] = '0';
        }
        buffer2[0] = '0';                                   //Colocar "0."
        buffer2[1] = '.';
        buffer2[4] = 0;                                     //Terminar string
        Lcd_Write_String(buffer2);                          //Escribir en LCD
        
        Lcd_Set_Cursor(2, 7);
        sprintf(buffer, "%d", AY);                          //Pasar a char
        if(AY > 0 && AY < 10){                              //Si es positivo menor a 10
            buffer2[3] = buffer[0];                         //Ubicar detras de "0.0"
            buffer2[2] = '0';
        }
        else if(AY > 0 && AY < 100){                        //Si es positivo menor a 100
            buffer2[3] = buffer[1];                         //Ubicar detras de "0."
            buffer2[2] = buffer[0];
        }
        else if(AY < 0 && AY > -10){                        //Si es negativo mayor a -10
            buffer2[3] = buffer[1];                         //Ubicar detras de "-0.0"
            buffer2[2] = '0';
            Lcd_Write_Char('-');
            Lcd_Set_Cursor(2, 8);
        }
        else if(AY < 0 && AY > -100){                       //Si es negativo mayor a -100
            buffer2[3] = buffer[2];                         //Ubicar detras de "-0."
            buffer2[2] = buffer[1];
            Lcd_Write_Char('-');
            Lcd_Set_Cursor(2, 8);
        }
        else{                                               //Si no se cumple
            buffer2[2] = '0';                               //Mostrar "0.00"
            buffer2[3] = '0';
        }
        buffer2[0] = '0';                                   //Colocar "0."
        buffer2[1] = '.';
        buffer2[4] = 0;                                     //Terminar string
        Lcd_Write_String(buffer2);                          //Escribir en LCD
        
        Lcd_Set_Cursor(2, 13);
        sprintf(buffer, "%d", Distance);                    //Pasar a char
        Lcd_Write_String(buffer);                           //Escribir en LCD
    }
}