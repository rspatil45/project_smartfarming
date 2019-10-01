var mqtt = require('mqtt')
const Influx = require('influx');
const express = require('express');
const _ = require('lodash');
const Influx2 = require('influxdb-nodejs');
const client2 = new Influx.InfluxDB('http://10.42.0.1:8086/smartagri')
const onHeaders = require('on-headers');
const app = express()
const port = 3001;




var buf;
let x;
var options = {
    host: "10.42.0.71",  //pc ip ###not sensor ip
    port: 1883,
};

var client = mqtt.connect(options)

client.on('connect', () => {
    console.log(`Connected to ${options.host} at ${options.port}`);
    client.subscribe('dht');
});

client.on('message', function (topic, message){
   // console.log(message.toString()); //if toString is not given, the message comes as buffer
    //console.log(message.toChar[0])
    //console.log("JSON "+message.toJSON(message)
   buf = message.toString();
    //console.log(buf)
    
    x = JSON.parse(buf);

    console.log(x);
    json = {
    "temperature": Number(x.temperature),
    "humidity": Number(x.humidity)
      }

  const influx = new Influx.InfluxDB({
  host: '10.42.0.1',
  database: 'smartagri',
  schema: [
    {
      measurement: 'sensordata',
      fields: {
        temperature: Influx.FieldType.FLOAT,
        humidity: Influx.FieldType.FLOAT
      },
      tags: [
        'host'
      ]
    }
  ]
})
//console.log(typeof(x))
console.log(x.temperature)

influx.writePoints([
  {
    measurement: 'sensordata',
    tags: { host: "cdac1" },
    fields: { temperature: Number(x.temperature), humidity: Number(x.humidity) },
  }
]).catch(err => {
  console.error(`Error saving data to InfluxDB! ${err.stack}`)
})


// console.log(json)

})

 
 const getData = (callback) => {
   client2.query('sensordata')
   .where('method', ['GET'])
   .then(callback)
   .catch(console.error);
   }
  
app.listen(port, () =>
     console.log(`Example app listening on port ${port}!`)
   )

app.get('/temphumid', (req, res) =>{client2.query(`
select * from sensordata`)
.then( result => res.status(200).json(result) )
.catch( error => res.status(500).json({ error }) );} )

app.get('/imagedata', (req, res) =>{client2.query(`
select * from imageData`)
.then( result => res.status(200).json(result) )
.catch( error => res.status(500).json({ error }) );} )
     
