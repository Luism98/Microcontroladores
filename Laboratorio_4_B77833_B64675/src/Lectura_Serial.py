import time
import csv
import serial
import paho.mqtt.client as mqtt
import json

#Estas funciones son tomadas de:  http://www.steves-internet-guide.com/thingsboard-mqtt-dashboard/

def on_log(client, userdata, level, buf):
   print(buf)

def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #Si la conexion es correcta
        print("connected OK") #Se imprime que la conexion es es correcta
    else:
        print("Bad connection Returned code=",rc) #Si la conexion es incorrecta
        client.loop_stop()  #Se detiene la conexion

def on_disconnect(client, userdata, rc): #Desconexion cliente
   print("client disconnected ok") #Se imprime que la desconexion es correcta

def on_publish(client, userdata, mid):
    print("In on_pub callback mid= ",mid)

#Protocolo de comunicacion

client = mqtt.Client('python3')
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish

mqtt.Client.connected_flag=False 
mqtt.Client.suppress_puback_flag=False

ser = serial.Serial('/tmp/ttyS1',9600,timeout=5) #Comunicacion con la placa mediante el puerto ttys0

token = "99xvllojuhgffZhciwal9"
topic = "v1/devices/me/telemetry"
topicreq = "v1/devices/me/attributes/request/1"

client.username_pw_set(token, password=None)

client.connect("iot.eie.ucr.ac.cr", 1883, 60) #--
while not(client.connected_flag):
	client.loop()
	time.sleep(1)
time.sleep(3)


data=dict()

for i in range(100): #Relizar un ciclo donde se añadan los datos obtenidos por el arduino
    info = ser.readline().decode().split(',')
    print(info)
    data["Humedad"]=info[0]
    data["Temperatura"]=info[1]
    data["Luz"]=info[2]
    data["Viento"]=info[3]
    data["Bateria"]=info[4] 
    data["Lluvia"]=info[5]  
    data_out=json.dumps(data) #Crear un objeto JSON necesario para IoT
    
    print("publish topic",topic, "data out= ",data_out)
    ret=client.publish(topic,data_out,0)    #publicar
    time.sleep(5)
 
    client.loop()
 
client.disconnect()



#Fuentes importantes: https://www.pythontutorial.net/python-basics/python-write-csv-file/