import requests
import picamera
import time
import datetime
from influxdb import InfluxDBClient
client = InfluxDBClient(host='192.168.77.114', port=8086)
client.switch_database('smartagri')
current_time = datetime.datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')



camera = picamera.PiCamera()

#address of cloud server where operations to be perform
api_host = 'http://192.168.77.114:6677/image'

headers = {'Content-Type' : 'image/jpeg'}
usr=input("Enter the farmer name ")
loc=input("Enter the location ")
while True:
    camera.capture('image1.jpg')
    data = open('image1.jpg','rb').read()
    
    response = requests.post(api_host, data=data)
   
    
    x = eval(response.text)
    l = x.get("predictions") 
    name = ""
    probability = ""
    variable = 0
    for temp in l:
        r = temp.get("probability")
        if float(r)>variable and float(r)<= 1:
            variable = temp.get("probability")
            name = temp.get("tagName")
            probability = temp.get("probability")

    print(" Name of disease : ", name)
    print(" Probability  is : ", probability)
    print(" (Note: probability max=1 and min=0)")
    print("___________________________________________")

    current_time = datetime.datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')
    json_body =[ {
            "measurement": "imageData",
            "tags":{
                "farmer_name": usr,
                 "disease_name": name
                },
            "time":current_time,
            "fields":{
                "location":loc,
                "probability":probability
                }
            }]
    client.write_points(json_body)
                     

x = """

 rsp rahul patil

"""
