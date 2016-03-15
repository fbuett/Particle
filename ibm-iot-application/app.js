/*eslint-env node*/

var stdin = process.openStdin();

//------------------------------------------------------------------------------
// node.js starter application for Bluemix
//------------------------------------------------------------------------------

// This application uses express as its web server
var express = require('express');

// create a new express server
var app = express();

// create a http server
var server = require('http').createServer(app);

// attach socket.io to http server
var io = require('socket.io').listen(server);

// use IoT Foundation
var Client = require("ibmiotf").IotfApplication;

// cfenv provides access to our Cloud Foundry environment
var cfenv = require('cfenv');

// get the app environment from Cloud Foundry
var appEnv = cfenv.getAppEnv();

// serve the files out of ./public as our main files
app.use(express.static(__dirname + '/public'));

// get credentials (ID, Org, API key & token) 
var config = {
  "org": 'l58oai',
  "id": 'particle-app',
  "auth-key": 'a-l58oai-2gg7lzhdv4',
  "auth-token": 'I3!OaNfiDMBSfwGdmB'
};
var appClient = new Client(config);

// connect to IoT Foundation
appClient.connect();

// subscribe to IoT devices events
appClient.on("connect", function () {

    appClient.subscribeToDeviceEvents();

    console.log("Hit <Enter> to fire event");
});

// start server on the specified port and binding host
server.listen(appEnv.port, function() {
  // print a message when the server starts listening
  console.log("server starting on " + appEnv.url);
  console.log(__dirname);
});

// listen to web client connect request
io.on('connection', function (socket) {
  	var counter = 0;
  	console.log('a user connected');
    
    // receive events from IoT devices  
  	appClient.on("deviceEvent", function (deviceType, deviceId, eventType, format, payload) {
		  console.log("Device Event from :: "+deviceType+" : "+deviceId+" of event "+eventType+" with payload : "+payload);
		
      // send event received from IoT devices to web client
      // socket.emit('news', "Device Event from :: "+deviceType+" : "+deviceId+" of event "+eventType+" with payload : "+payload+" Counter : "+counter);
      socket.emit('news', String(payload));
      counter++;
    });
  
    // receive messages and commands from web client
    socket.on('my other event', function (data) {
      console.log(data);
    });
});

stdin.on('data', function(chunk) {
  var myData=String("{\"DelaySeconds\" : 10}");

  appClient.publishDeviceCommand("Photon","2b002f000547343233323032", "command", "json", myData);

  console.log("Fire!");
});




