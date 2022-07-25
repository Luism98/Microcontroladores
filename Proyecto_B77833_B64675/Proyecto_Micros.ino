#include <math.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> // cambiar

//Declaración de pines

//LEDS 
const int Luz_USART=2;
const int Luz_Alarma=3;
//Pantalla SSD1306
https://programarfacil.com/blog/arduino-blog/ssd1306-pantalla-oled-con-arduino/
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); 

//Comunicacion USART
const int comunicacion=53;
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
//Nivel de batería 
const int Bateria=A7;
// Activar Pantalla
const int activacion_pantalla=51;
// Activar Pantalla
const int activacion_usart=53;
int contador=0;
//Servomotores
const int ajuste_eje_x=A2;
const int ajuste_eje_y=A3;
Servo EjeX;
Servo EjeY;
//Ajuste de panel solar


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
  pinMode(comunicacion, INPUT);
  pinMode(Pin_termistor, INPUT);
  pinMode(Sensor_Luminocidad, INPUT);
  pinMode(Bateria, INPUT);
 // Setup de la pantalla SSD1306 ***************
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C)
  // init done
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();   // Limpia la pantalla 
 //SERVOMOTORES
  pinMode(ajuste_eje_x, INPUT);
  pinMode(ajuste_eje_y, INPUT);
  EjeX.attach(9);
  EjeY.attach(10);

}
void control_servos(){
	EjeX.write((0.175953)*analogRead(ajuste_eje_x));
	EjeY.write((0.175953)*analogRead(ajuste_eje_y));
}

void Alerta_bateria_baja(){
	if(analogRead(Bateria)/10.23<=25)
	{
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
	
	if (digitalRead(activacion_usart == HIGH){
		if(time - USART_timer >600000){
		
			USART_timer = time;
			Serial.print("Humedad:");
        		Serial.println(analogRead(Sensor_Humedad)/10.23);
	      	Serial.print("Temperatura:");
        		Serial.println(Temperatura_Termistor());
	      	Serial.print("Intensidad de luz:");
        		Serial.println(Intensidad_Luz(analogRead(Sensor_Luminocidad)));
        	
		}
	
	}
}





void loop() { //Loop
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
    //Nivel de batería 
      display.print("N_B:");
	 display.print(analogRead(Bateria)/10.23); //Nivel de bateríá simulado con fuente de 12 V.
	 display.print(" %"); //m sobre s
	 display.print("\n"); 
	 

     delay(500);
     display.display();
	display.clearDisplay();
    }
    else { //Pantalla apagada
	display.display();
	}
    //Servomotores
	control_servos();
    //Ajuste de panel solar
    // Alarma Bateria baja
    Alerta_bateria_baja();
    // USART ACTIVA
	USART_comm(); // comunicacion de datos
     USART_Activa(); // parpadeo
}