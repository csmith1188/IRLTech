const express = require('express');
var app = express();
const path = require('path');
const session = require('express-session');
const SQLiteStore = require('connect-sqlite3')(session);
const http = require('http').Server(app);
const io = require('socket.io')(http);
const route = require('./modules/routes.js');
const socketHandle = require('./modules/socket.js');
const crypto = require('crypto');
const jwt = require('jsonwebtoken');

const sqlite3 = require('sqlite3').verbose();
const db = new sqlite3.Database('./data/database.db', (err) => {
    if (err) {
        return console.error(err.message);
    } else {
        console.log('Connected to the database.');
    }
});


const sessionMiddleware = session({
    store: new SQLiteStore,
    secret: 'games are funzy',
    resave: false,
    saveUninitialized: true,
    cookie: { secure: false } // Set to true if using HTTPS
})

const port = 3000;

app.set('view engine', 'ejs');

// Middleware
app.use(sessionMiddleware)
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static(path.join(__dirname, 'public')));

io.use((socket, next) => { sessionMiddleware(socket.request, {}, next); });

// Routes
app.get('/', route.getRoot);

app.get('/login', route.getLogin);

app.post('/login', route.postLogin);

app.get('/logout', route.logout);

app.get('/profile', route.isAuthenticated, route.getProfile);

io.on('connection', socketHandle.connection);

// Start server
http.listen(port, (err) => {
    if (err) {
        console.log(err);
    } else {
        console.log(`Server is running on http://localhost:${port}`);
    }
});