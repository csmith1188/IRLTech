const express = require('express');
const socketIo = require('socket.io');
const ejs = require('ejs');
const fs = require('fs');
const socket = require('./modules/socket.js');
// const main = require('./modules/main.js');
const app = express(); 
PORT = 3000;

path = require('path');
const routes = require('./modules/routes.js');
app.set('view engine', 'ejs');
app.use(express.urlencoded({ extended: true }));
app.use(express.static('public'));
const server = app.listen(PORT, () => {console.log(`Server running on port ${PORT}`);});
const io = socketIo(server);

app.get('/', routes.index);

io.on('connection', socket.socketH);