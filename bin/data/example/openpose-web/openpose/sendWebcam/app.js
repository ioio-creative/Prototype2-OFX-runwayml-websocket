const express = require('express');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);

const protocol = process.env.HTTPS === 'true' ? 'https' : 'http';
const HOST = process.env.HOST || 'localhost';
const port = process.env.PORT || 3000;
var opener = require("opener");


var url = protocol +'://'+ HOST + ':' + port;
opener(url);
console.log("URL" + url)

app.use(express.static(__dirname + '/public'));

function onConnection(socket){
  socket.on('drawing', (data) => socket.broadcast.emit('drawing', data));
}

io.on('connection', onConnection);

http.listen(port, () => console.log('listening on port ' + port));
