#include <EEPROM.h>
#include <math.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <Servo.h>
#include <Adafruit_PCD8544.h>
#include <Keypad.h>
//Declaración de pines
//KEYPAD
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {43,41,39,37}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {31, 33, 35}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

const String contra = "12"; // change your password here
String input_contra; 
//LEDS 
const int Luz_Alar=2;
const int Luces_Auto=3;
const int Luz_Bateria=4;
//Pantalla PCD8544
// Informacion extraída de: https://www.electrosoftcloud.com/pantalla-lcd-nokia-5110-con-arduino-parte-2/
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);

//Comunicacion USART
unsigned long USART_timer; 
//Sensor de Luminocidad
const int Sensor_Luminocidad=A1;
//Nivel de batería 
const int Bateria=A7;
// Sensor Alarma
const int intruso=50;
int contador=0;
//AC
const int Ajuste_AC=A2;
//CAMARAS
Servo EjeX;
Servo EjeY;
unsigned long time;
//BOCINA
const int bocina=13; 
//Sensor Ultrasonico
unsigned long duracion;
long distancia;
int echo=11;
int trig=12;
//Estado Seguridad
bool Seguridad=false;
// Encendido
const int Boton_Encendido=22;
bool Encendido=false;





double Intensidad_Luz(int AnVal){ // Basado en https://forum.arduino.cc/t/light-intensity-code/51127

  double Vout= AnVal*0.0048828125;
  int lux = (500/((10.72/(5-Vout))*Vout));
  return lux;
  }

void setup() {
  Serial.begin(9600); // Inicio del puerto serial
 // Configuracion de pines como salida
  pinMode(Luz_Alar,OUTPUT);
  pinMode(Luces_Auto,OUTPUT);
  pinMode(Luz_Bateria,OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(Boton_Encendido,INPUT);
  pinMode(echo,INPUT);
  pinMode(Sensor_Luminocidad, INPUT);
  pinMode(Bateria, INPUT);
  pinMode(intruso, INPUT);
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
  pinMode(Ajuste_AC, INPUT);
  EjeX.attach(9);
  EjeY.attach(10);




}
void control_servos(){
	if(digitalRead(intruso)==HIGH){
		EjeX.write((0.175953)*250);
		EjeY.write((0.175953)*700);
	
	}
	else{
	EjeX.write((0.175953)*1023);
	EjeY.write((0.175953)*0);
	}
}

void Alerta_bateria_baja(){
	if(analogRead(Bateria)/10.23<=25)
	{
	digitalWrite(Luz_Bateria, HIGH);
	 delay(500);
	 digitalWrite(Luz_Bateria, LOW);
	}
	else digitalWrite(Luz_Bateria, LOW); 
}

void USART_comm(){ // comunicacion USART 
	if(time - USART_timer >10000){
	
		USART_timer = time;

       	Serial.print(17+(0.0175953)*analogRead(Ajuste_AC)); //Luminocidad
		Serial.print(",");
		Serial.print(analogRead(Bateria)/10.23);//Nivel Bateria
		Serial.print(",");
 		if(digitalRead(intruso) == HIGH){ 
			Serial.print("NO"); //Intruso NO
			Serial.print(","); 
		} 
		else {
			Serial.print("SI"); //Intruso SI
			Serial.print(","); 	
			}
 		if(Encendido){ 
			Serial.print("SI"); //Intruso NO
			Serial.print(","); 
		} 
		else {
			Serial.print("NO"); //Intruso SI
			Serial.print(","); 	
			}
 		if(Seguridad){ 
			Serial.print("ACTIVA"); //Intruso NO
			Serial.print(","); 
		} 
		else {
			Serial.print("DESACTIVADA"); //Intruso SI
			Serial.print(","); 	
			}
	}
}

void pitido(){
	tone(bocina,2000);
}


void loop() { //Loop
	char key = keypad.getKey();
	time = millis();
	// Lectura Ultrasonico
	digitalWrite(trig,LOW);
	delayMicroseconds(4);
	digitalWrite(trig,HIGH);
	delayMicroseconds(10);
	digitalWrite(trig,LOW);
	duracion=pulseIn(echo,HIGH);
	distancia=duracion/58.4;
	

	//__________CARRO INTELIGENTE____________
	if(Encendido){
		//SENSOR PROXIMIDAD
		if(distancia<=10.0){
		pitido();
		}
		else{
			noTone(bocina);
		}
		//ENCENDIDO LUCES AUTOMÁTICO
		if(Intensidad_Luz(analogRead(Sensor_Luminocidad))<=1000)
		digitalWrite(Luces_Auto, HIGH);
		//AIRE ACONDICIONADO
		else{
		digitalWrite(Luces_Auto, LOW);
		}		
		
	}
	else{
		digitalWrite(Luces_Auto, LOW);
		if(Seguridad){
			if(digitalRead(intruso)==LOW){
			pitido();
			digitalWrite(Luz_Alar, HIGH);
			delay(500);
			digitalWrite(Luz_Alar, LOW);
			}
			else{
				noTone(bocina);
				digitalWrite(Luz_Alar, LOW);
			}	
		}
		else{
			display.print("Seg_OFF \n");
			noTone(bocina);
			digitalWrite(Luz_Alar, LOW);
			
		}
	}
	display.print("Pass:"+input_contra+"\n");
	if (key) {
		Serial.println(key);       
		if(key == '*') {                    
			input_contra = ""; // limpiar    
		} 
		else if(key == '#') {
			if(contra == input_contra) {
			display.print("C_Co \n");
			delay(10000);
			Seguridad = !Seguridad;
			input_contra = "";
			//display.clearDisplay();
			} else {
			//display.clearDisplay();
			display.print("C_Inco \n"); 
			input_contra = "";
			delay(10000);
			//display.clearDisplay();
			}
		} else {
			input_contra += key;
		}
	}

	if(digitalRead(Boton_Encendido)== HIGH){
		Encendido=true;
	}
	else{
		Encendido=false;
	}


	//__________PANTALLA____________
    // Distancia
	display.print("Dist:");
	display.print(distancia); //Temperatura Termistor
	display.print("cm"); 
	display.print("\n");
	display.print("L:");
	display.print(Intensidad_Luz(analogRead(Sensor_Luminocidad))); //Temperatura Termistor
	display.print("lx"); 
	display.print("\n");
	display.print("AC:");	
	display.print(17+(0.0175953)*analogRead(Ajuste_AC)); //Temperatura Termistor
	display.print("C"); 
	display.print("\n");		 
    delay(500);
    display.display();
	display.clearDisplay();


	//Servomotores
	control_servos();
    // Alarma Bateria baja
    Alerta_bateria_baja();
    // USART
	USART_comm(); // comunicacion de datos1

}