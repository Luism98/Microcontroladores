#include <EEPROM.h>
#include <math.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Servo.h>
#include <LowPower.h>
#include <Adafruit_PCD8544.h>

//Declaración de pines

//LEDS 
const int Luz_USART=2;
const int Luz_Alarma=3;
//Pantalla PCD8544
// Informacion extraída de: https://www.electrosoftcloud.com/pantalla-lcd-nokia-5110-con-arduino-parte-2/
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);

//Comunicacion USART
unsigned long USART_timer; 
//Sensor Humedad
const int Sensor_Humedad= A5;
//Sensor de Temperatura
const int Pin_termistor=A0;
int Vo=0;
const float R1=10000;
float logRT, RT, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
//Sensor de Luminocidad
const int Sensor_Luminocidad=A1;
//Sensor de velocidad de viendo
const int Velocidad_viento=A4;
//Nivel de batería 
const int Bateria=A7;
// Sensor Lluvia
const int Lluvia=49;
// Activar Pantalla
const int activacion_pantalla=51;
// Activar USART
const int activacion_usart=53;
int contador=0;
//Servomotores
const int ajuste_eje_x=A2;
const int ajuste_eje_y=A3;
Servo EjeX;
Servo EjeY;
//Ajuste de panel solar

//EEPROM
int EEPROM_adr = 0;
unsigned long EEPROM_timer;
unsigned long time;


float Temperatura_Termistor(){ // Informacion extraída de: https://www.circuitbasics.com/arduino-thermistor-temperature-sensor-tutorial/ 
 Vo= analogRead(Pin_termistor);
 RT = R1 * (1023.0 / (float)Vo - 1.0);
 logRT = log(RT);
 T = (1.0 / (c1 + c2*logRT + c3*logRT*logRT*logRT));
 T = T - 273.15; // Temperatura en Celsius
 return T;
}

double Intensidad_Luz(int AnVal){ // Basado en https://forum.arduino.cc/t/light-intensity-code/51127

  double Vout= AnVal*0.0048828125;
  int lux = (500/((10.72/(5-Vout))*Vout));
  return lux;
  }

void setup() {
  Serial.begin(9600); // Inicio del puerto serial
 // Configuracion de pines como salida
  pinMode(Luz_USART,OUTPUT);
  pinMode(Luz_Alarma,OUTPUT);
  pinMode(Pin_termistor, INPUT);
  pinMode(Sensor_Luminocidad, INPUT);
  pinMode(Velocidad_viento, INPUT);
  pinMode(Bateria, INPUT);
  pinMode(Lluvia, INPUT);
  pinMode(activacion_pantalla, INPUT);
  pinMode(activacion_usart, INPUT); 
 // Setup de la pantalla PCD8544
  display.begin();
  // init done
  display.clearDisplay();
  display.display();
  display.setContrast(50);
  display.setCursor(0,0);
  display.setTextColor(BLACK);
  display.clearDisplay();   // Limpia la pantalla 
 //SERVOMOTORES
  pinMode(ajuste_eje_x, INPUT);
  pinMode(ajuste_eje_y, INPUT);
  EjeX.attach(9);
  EjeY.attach(10);
 //EEPROM
 reset_EEPROM();


}
void control_servos(){
	EjeX.write((0.175953)*analogRead(ajuste_eje_x));
	EjeY.write((0.175953)*analogRead(ajuste_eje_y));
}

void Alerta_bateria_baja(){
	if(analogRead(Bateria)/10.23<=25)
	{
	LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); //MODO AHORRO DE ENERGIA		
	digitalWrite(Luz_Alarma, HIGH);
	 delay(500);
	 digitalWrite(Luz_Alarma, LOW);
	}
	else digitalWrite(Luz_Alarma, LOW); 
}
void USART_Activa(){
	if(contador<5){ // parpadeo
	if(digitalRead(activacion_usart)==HIGH){
	 digitalWrite(Luz_USART, HIGH);
	 delay(200);
	 digitalWrite(Luz_USART, LOW);
	 contador++;
	}
	else digitalWrite(Luz_USART, LOW);
	}
	else {
	delay(1000);
	contador=0;
	}
	 
}

void USART_comm(){ // comunicacion USART a los 10 min
	
	if (digitalRead(activacion_usart) == HIGH){
		if(time - USART_timer >600000){
		
			USART_timer = time;
        		Serial.print(analogRead(Sensor_Humedad)/10.23); //Humedad
			Serial.print(",");
        		Serial.print(Temperatura_Termistor()); //Temperatura
			Serial.print(",");
        		Serial.print(Intensidad_Luz(analogRead(Sensor_Luminocidad))); //Luminocidad
			Serial.print(",");
        		Serial.print((0.029325)*analogRead(Velocidad_viento)); //Viento
			Serial.print(",");
			Serial.print(analogRead(Bateria)/10.23);//Nivel Bateria
			Serial.print(",");
	 			if(digitalRead(Lluvia) == HIGH){ 
	 				Serial.print("NO"); //Esta lloviendos
					Serial.print(","); 
	 			} else {
	 				Serial.print("SI"); //No esta lloviendo
					Serial.print(","); 	
	 			} 
		}
	
	}
}

void reset_EEPROM(){
	for (int i=0; i < EEPROM.length(); i++){
		EEPROM.write(i,0); // reemplaza datos al escribir ceros
	}
}

void EEMPROM_checkadr(){

	if (EEPROM_adr == EEPROM.length()){
		EEPROM_adr = 0;
		reset_EEPROM();
	}

}

void write_EEPROM(){
	if (time - EEPROM_timer > 300000){

	// Esribir sensor de lluvia
	EEMPROM_checkadr();
	EEPROM.write(EEPROM_adr, digitalRead(Lluvia));
	++EEPROM_adr;
	// Escribir sensor de humedad
	EEMPROM_checkadr();
	EEPROM.write(EEPROM_adr, analogRead(Sensor_Humedad)/10.23);
	++EEPROM_adr;
	//Escribir sensor de temperatura
	EEMPROM_checkadr();
	EEPROM.write(EEPROM_adr, Temperatura_Termistor());
	++EEPROM_adr;
	//Escribir sensor de luz
	EEMPROM_checkadr();
	EEPROM.write(EEPROM_adr, Intensidad_Luz(analogRead(Sensor_Luminocidad)));
	++EEPROM_adr;
	// Escribir sensor de velocidad de viento
	EEMPROM_checkadr();
	EEPROM.write(EEPROM_adr,(0.029325)*analogRead(Velocidad_viento));
	++EEPROM_adr;
	
	}
}




void loop() { //Loop
	time = millis();
     // Temperatura
	if(digitalRead(activacion_pantalla)==HIGH){ //Pantalla encendida
     display.print("Temp:");
	 display.print(Temperatura_Termistor()); //Temperatura Termistor
	 display.print("C"); 
	 display.print("\n");
	 //Porcentaje de Humedad
	 display.print("Hum:");
	 display.print(analogRead(Sensor_Humedad)/10.23); //Porcentaje de Humedad simulado con la fuente de voltaje de 5 V.
	 display.print("%"); 
	 display.print("\n");
     //Sensor de Luminocidad
     display.print("I_L:");
	 display.print(Intensidad_Luz(analogRead(Sensor_Luminocidad))); // Lectura intensidad luminica en lux
	 display.print(" lx"); //LUX
	 display.print("\n");
    //Sensor de velocidad de viendo
      display.print("V_V:");
	 display.print((0.029325)*analogRead(Velocidad_viento)); //Velocidad de viento simulado con la fuente de voltaje de 5 V. de 0 ms a 30ms
	 display.print(" m/s"); //m sobre s
	 display.print("\n");
    //Nivel de batería 
      display.print("N_B:");
	 display.print(analogRead(Bateria)/10.23); //Nivel de bateríá simulado con fuente de 12 V.
	 display.print(" %"); //m sobre s
	 display.print("\n");
    //Sensor LLuvia
	 display.print("Lluvia:");
	 
	 if(digitalRead(Lluvia) == HIGH){ 
	 display.print("NO"); //Esta lloviendo
	 display.print("\n");
	 }
	 else {
	 display.print("SI"); //No esta lloviendo
	 display.print("\n");
	 } 
     delay(500);
     display.display();
	display.clearDisplay();
    }
    else { //Pantalla apagada
	display.display();
	}
    //Servomotores
	control_servos();
    // Alarma Bateria baja
    Alerta_bateria_baja();
    // USART ACTIVA
	USART_comm(); // comunicacion de datos
     USART_Activa(); // parpadeo
	// EEPROM
	write_EEPROM();
}