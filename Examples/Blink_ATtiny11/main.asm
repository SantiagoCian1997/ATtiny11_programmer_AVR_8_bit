;
; Blink_ATtiny11.asm
;
; Created: 11/3/2023 22:07:54
; Author : Santiago PC
;

.def cont_senal=r20
.def cont_retardo=r19
#define  retardo 50 //cantidad de operaciones nulas que se hacen para generar el retardo
; parpadeo de todas las salidas a diferentes frecuencias, o tambien se puede ver como contador binario de 5 bits
start:

	sbi DDRB,PB0 //como salida
	sbi DDRB,PB1 //como salida
	sbi DDRB,PB2 //como salida
	sbi DDRB,PB3 //como salida
	sbi DDRB,PB4 //como salida
	
	ldi cont_retardo,0	
	ldi cont_senal,0	

loop:

/*				//blink generico
	sbi PORTB, PB0
	nop
	cbi PORTB, PB0
	nop
    rjmp loop
*/

				//blink como contador binario
	inc cont_senal			//incremento el contador que genera la señal
	out PORTB,cont_senal	//asigno el contador al registro de salida
	
	ldi cont_retardo,0	
delay:
	cpi cont_retardo,retardo	//comparo el contador de retardo con el valor de retardo
	breq out_delay
	inc cont_retardo			//cont_retardo++
	nop							//operaciones nulas para el retardo
	nop
	rjmp delay
out_delay:

    rjmp loop

