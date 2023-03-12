;
; AssemblerApplication2.asm
;
; Created: 24/2/2021 15:37:21
; Author : Santiago PC
;


; Este ejemplo hace una animacion con cuatro salidas PWM generado por software en los pines PB0, PB2, PB3 y queda bocetado la salida PWM para el pin PB4. Cambiando de estado el pin PB1 se puede elegir la velocidad de la animacion
.def PWM0=r16 ;PB0
.def PWM1=r17 ;PB2
.def PWM2=r18 ;PB3
.def PWM3=r19 ;PB4
.def contPWM=r20
.def contCiclos=r21 ; cada vez que se reinicie contPWM se incrementa y es mi base apra las animaciones
.def retardo=r22; numero de contCiclos para que se realice un cambio
.def animacion=r23; numero de animacion a ejecutar
.def aux=r24

start:
.org 0x0000
rjmp RESET_INT
.org 0x0001
rjmp EXT_INT0 //vector de interrupciones
RESET_INT:
	in r16,SREG
	ori r16,0b10000000
	out SREG,r16//habilitar interrupciones

	in r16,GIMSK
	ori r16,0b01000000
	out GIMSK,r16//hanbilito interrupcion externa

	sbi DDRB,PB0 //como salida
	sbi DDRB,PB2 //como salida
	sbi DDRB,PB3 //como salida
	sbi DDRB,PB4 //como salida
	ldi PWM0,255
	ldi PWM1,0
	ldi PWM2,0
	ldi retardo,2
	ldi animacion,0

loop:
	//cambio de animacion
	in aux,PINB
	andi aux,0b10000
	//fin cambio animacion

	cp contCiclos,retardo; si contCiclo==retardo
	brNE outCambioAnim
	ldi contCiclos,0; ciclos=0
	/////////////////////// cambio de animaciones
	cpi animacion,0//animacion 0
	brNE outAnim0
	
	cpi PWM0,255   //if pwm0==255,pwm2==0,pwm1!=255
	brNE outT1
	cpi PWM2,0
	brNE outT1
	cpi PWM1,255
	brEQ outT1
	inc PWM1
outT1:

	cpi PWM1,255   //if pwm1==255,pwm2==0,pwm0!=0
	brNE outT2
	cpi PWM2,0
	brNE outT2
	cpi PWM0,0
	brEQ outT2
	dec PWM0
outT2:

	cpi PWM1,255   //if pwm1==255,pwm0==0,pwm2!=255
	brNE outT3
	cpi PWM0,0
	brNE outT3
	cpi PWM2,255
	brEQ outT3
	inc PWM2
outT3:

	cpi PWM2,255   //if pwm2==255,pwm0==0,pwm1!=0
	brNE outT4
	cpi PWM0,0
	brNE outT4
	cpi PWM1,0
	brEQ outT4
	dec PWM1
outT4:

	cpi PWM2,255   //if pwm2==255,pwm1==0,pwm0!=255
	brNE outT5
	cpi PWM1,0
	brNE outT5
	cpi PWM0,255
	brEQ outT5
	inc PWM0
outT5:

	cpi PWM0,255   //if pwm0==255,pwm1==0,pwm2!=0
	brNE outT6
	cpi PWM1,0
	brNE outT6
	cpi PWM2,0
	brEQ outT6
	dec PWM2
outT6:

outAnim0://fin animacion 0
	cpi animacion,1//animacion 1
	brNE outAnim1

outAnim1://fin animacion 1
	cpi animacion,2//animacion 2
	brNE outAnim2

outAnim2://fin animacion 2
	cpi animacion,3//animacion 3
	brNE outAnim3

outAnim3://fin animacion 3
	/////////////////////// fin cambio de animaciones
outCambioAnim:

	cpi contPWM,0; cada ciclo completo de PWM se aumenta contCiclo
	brNE outIncremetoCiclo
	inc contCiclos ;ciclo++
outIncremetoCiclo:
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

	cp contPWM,PWM3
	brLO onPWM3
	cbi PORTB,PB4
	rjmp outPWM3
onPWM3:
	sbi PORTB,PB4
outPWM3:

    rjmp loop
	



	/// cambio de velocidad
EXT_INT0:
	cpi retardo,1
	brNE outR1
	ldi retardo,2
	reti
outR1:
	cpi retardo,2
	brNE outR2
	ldi retardo,4
	reti
outR2:
	cpi retardo,4
	brNE outR3
	ldi retardo,6
	reti
outR3:
	cpi retardo,6
	brNE outR4
	ldi retardo,10
	reti
outR4:
	cpi retardo,10
	brNE outR5
	ldi retardo,20
	reti
outR5:
	cpi retardo,20
	brNE outR6
	ldi retardo,35
	reti
outR6:
	cpi retardo,35
	brNE outR7
	ldi retardo,50
	reti
outR7:
	cpi retardo,50
	brNE outR8
	ldi retardo,80
	reti
outR8:
	cpi retardo,80
	brNE outR9
	ldi retardo,110
	reti
outR9:
	cpi retardo,110
	brNE outR10
	ldi retardo,170
	reti
outR10:
	cpi retardo,170
	brNE outR11
	ldi retardo,200
	reti
outR11:
	cpi retardo,200
	brNE outR12
	ldi retardo,1
	reti
outR12:
	reti