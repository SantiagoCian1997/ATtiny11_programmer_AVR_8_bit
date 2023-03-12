;
; Test_HEX.asm
;
; Created: 26/12/2020 16:27:07
; Author : Santiago PC
;
.def aux =r16
.def PWM0=r17 ; va al PB0
.def PWM1=r18 ; va al PB2
.def PWM2=r19 ; va al PB3
.def contPWM=r20 
.def contCicl=r21 
.def dato=r22  ;almaceno datos de entrada ir
; Este ejemplo genera tres señales PWM por software a partir de la informacion srial que llega al pin de entrada PB1, la trama indica la direccion del dispositivo al cual este responde y los tres bits sucesivos correspondientes al ancho del pulso de PWM de cada salida
start:
.org 0x0000
rjmp RESET_INT
.org 0x0001 //vector de interrupciones
rjmp EXT_INT0
RESET_INT:
	in r16,SREG
	ori r16,0b10000000
	out SREG,r16//habilitar interrupciones

	in r16,GIMSK
	ori r16,0b01000000
	out GIMSK,r16//hanbilito interrupcion externa

	in  r16,MCUCR
	ori r16,0b00000001  //ISC01=0 y ISC00=1 para saltar por cualquier cambio
	out MCUCR,r16//configuro interrupcion por subida y por bajada

	sbi DDRB,PB0 //como salida
	sbi DDRB,PB2 //como salida
	sbi DDRB,PB3 //como salida
	ldi PWM0,10
	ldi PWM1,10
	ldi PWM2,0
	ldi aux,0
	ldi contPWM,0
	ldi contCicl,0



loop:
	mov PWM2,dato

	inc contCicl
	inc contPWM
	
	cp contPWM,PWM0
	brLO onPWM0
	cbi PORTB,PB0
	rjmp outPWM0
onPWM0:
	sbi PORTB,PB0
outPWM0:

	cp contPWM,PWM1
	brLO onPWM1
	cbi PORTB,PB2
	rjmp outPWM1
onPWM1:
	sbi PORTB,PB2
outPWM1:

	cp contPWM,PWM2
	brLO onPWM2
	cbi PORTB,PB3
	rjmp outPWM2
onPWM2:
	sbi PORTB,PB3
outPWM2:

    rjmp loop

EXT_INT0:
	in aux,PINB
	andi aux,0b10//verifico estado de PB1
	cpi aux,0
	brEQ flancoBajada
flancoSubida:
	lsl dato  //desplazo a la izquierda
	cpi contCicl,10
	brLO setDato
	reti
setDato:
	ori dato,0b1// seteo el bit menos significativo si el tiempo en bajo es menor que diez ciclos
	reti
flancoBajada:	
	ldi contCicl,0 //reset cont

	reti