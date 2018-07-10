var osc = require('node-osc');
var oscServer,
  oscClient;
var ioOsc = require('socket.io')(8081);
var isConnected = false;

const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

const protocol = process.env.HTTPS === 'true'
  ? 'https'
  : 'http';
const HOST = process.env.HOST || 'localhost';
const port = process.env.PORT || 3000;
var opener = require("opener");

var url = protocol + '://' + HOST + ':' + port;
opener(url);
console.log("URL" + url)

app.use(express.static(__dirname + '/public'));

function onConnection(socket) {
  socket.on('drawing', (data) => socket.broadcast.emit('drawing', data));

}

io.on('connection', onConnection);

http.listen(port, () => console.log('listening on port ' + port));

ioOsc.sockets.on('connection', function(socketOSC) {
  console.log('connection');
  socketOSC.on("config", function(obj) {
    isConnected = true;
    oscServer = new osc.Server(obj.server.port, obj.server.host);
    oscClient = new osc.Client(obj.client.host, obj.client.port);
    oscClient.send('/status', socketOSC.sessionId + ' connected');
    oscServer.on('message', function(msg, rinfo) {
      socketOSC.emit("message", msg);
    });
    socketOSC.emit("connected", 1);
  });
  socketOSC.on("messageSend", function(obj) {
    console.log("reach here")
    oscClient.send.apply(oscClient, obj);
  });
  socketOSC.on('disconnect', function() {
    if (isConnected) {
      oscServer.kill();
      oscClient.kill();
    }
  });
});
