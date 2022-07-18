import time
import csv
import serial

ser = serial.Serial('/dev/ttyACM0',115200,timeout=5) #Comunicacion con la placa mediante el puerto ttys0

#data=dict()
'''
for i in range(100): #Relizar un ciclo donde se añadan los datos obtenidos por el arduino
    info = ser.readline().decode().split(':')
    print(info)
    data["X"]=info[0]
    data["Y"]=info[1]
    data["Z"]=info[2]

    
    print("X",info[0], "Y",info[1], "Z", info[2])
    time.sleep(5)
''' 

''' 
while True:
try:
    ser_bytes = ser.readline()
    // the following line splits the line you got from your arduino into an array
    decoded_bytes = ser_bytes.split(",");
    print(decoded_bytes)

    with open("test_data.csv","a") as f:
        writer = csv.writer(f,delimiter=",")
        writer.writerow(decoded_bytes)
'''        


#Fuentes importantes: https://www.pythontutorial.net/python-basics/python-write-csv-file/