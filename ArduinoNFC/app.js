const express = require('express');
const socketIo = require('socket.io');
const session = require('express-session');
const ejs = require('ejs');
const fs = require('fs');
const crypto = require('crypto');
const jwt = require('jsonwebtoken');
const SQLiteStore = require('connect-sqlite3')(session);
const socket = require('./modules/socket.js');
const routes = require('./modules/routes.js');
// const main = require('./modules/main.js');
const app = express();
PORT = 3000;
path = require('path');
app.set('view engine', 'ejs');
app.use(express.urlencoded({ extended: true }));
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));
const server = app.listen(PORT, () => { console.log(`Server running on port ${PORT}`); });
const io = socketIo(server);
const sessionMiddleware = session({
    store: new SQLiteStore,
    secret: 'games are funzy',
    resave: false,
    saveUninitialized: true,
    cookie: { secure: false } // Set to true if using HTTPS
})
app.use(sessionMiddleware)
io.use((socket, next) => { sessionMiddleware(socket.request, {}, next); });

app.get('/', routes.isAuthenticated, routes.index);

io.on('connection', socket.socketH);

app.get('/area', routes.getRoot);

app.get('/login', routes.getLogin);

app.post('/login', routes.postLogin);

app.get('/logout', routes.logout);

app.get('/profile', routes.isAuthenticated, routes.getProfile);