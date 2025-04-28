const express = require('express')
const sqlite3 = require('sqlite3')
const session = require('express-session')
const app = express()

const { SerialPort } = require('serialport'); // Import the serialport module
const { ReadlineParser } = require('@serialport/parser-readline'); // Import the parser module
const readline = require('readline').createInterface({
  input: process.stdin,
  output: process.stdout
});

let port;
let pendingCommand;

SerialPort.list()
  .then((ports) => {
    // Create a new SerialPort instance
    port = new SerialPort({
      path: 'COM3',
      baudRate: 115200,
      autoOpen: true,
    });
    // If there's a pending command, write it to the port
    if (pendingCommand) {
      port.write(pendingCommand + '\n');
      pendingCommand = null;
    }

    let isCommandPrompted = false;

    // Create a new parser instance
    const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))

    // Open the port
    port.on("open", () => {
      console.log('Serial port open');
    });

    // Read the port data
    parser.on('data', data => {
      if (data.trim() !== 'Here are some commands that can be used: read, write, erase') {
        console.log(data);
       
      }
      if (!isCommandPrompted) {
        setTimeout(() => {
          readline.question('Commands: \n To write to specific records \n write \n To read specific records \n r0/  \n r1/ \n r2/ \n r3/ \n', command => {
            if (command) {
              var sendMessage = command + '\n'
              port.write(sendMessage)
              // readline.close();
            }
          });
          isCommandPrompted = true;
        }, 1000); // Delay of 1 second
      }
    });
  })
  .catch((err) => console.log(err));

  function writeToPort(command, callback) {
    if (port) {
        port.write(command + '\n');
        if (callback && typeof callback === 'function') {
            port.once('data', callback);
        }
    } else {
        // If the port is not initialized yet, store the command
        pendingCommand = command;
    }
}

function readFromPort(command, callback) {
  if (port) {
      let dataChunks = '';
      port.write(command + '\n');
      port.on('data', (data) => {
          dataChunks += data.toString();
          if (data.toString().endsWith('\n')) {
              callback(dataChunks);
              port.removeAllListeners('data'); // remove the listener once we're done
          }
      });
  } else {
      // If the port is not initialized yet, store the command
      pendingCommand = command;
  }
}

app.use(session({
    secret: 'Ths s nt my dg',
    saveUninitialized: false,
    resave: false
}))

let db = new sqlite3.Database("data.db", (err) => {
    if (err) {
        console.error(err)
    } else {
        console.log('Database running')
    }
})

app.use(express.urlencoded({extended: true}))

function isAuthenticated(req, res, next) {
    if (req.session.num) {
        next()
    } else {
        res.redirect('login')
    }
}

app.set("view engine", "ejs")

app.get('/', isAuthenticated, (req, res) => {
    db.get("SELECT * FROM users WHERE uid=?", req.session.num, (err, row) => {
        if (row) {
            console.log(row)
            res.render('index', {
                user: row.name,
                accountNum: row.accountNum,
                balance: row.balance
            })
        } else {
            console.error(err)
        }
    })
})

app.get('/transaction', isAuthenticated, (req, res) => {
    db.get("SELECT * FROM users WHERE uid=?", req.session.num, (err, row) => {
        if (row) {
            console.log(row)
            res.render('transaction', {
                user: row.name,
                accountNum: row.accountNum,
                balance: row.balance
            })
        } else {
            console.error(err)
        }
    })
})

app.post('/transaction', isAuthenticated, (req, res) => {
    let receiverNumber;
    readFromPort("r0/", (data) => {
        receiverNumber = data
    })
    console.log(receiverNumber)
    let amount = req.body.amount
    let account = req.body.accountNumber
    db.run("UPDATE users SET balance = balance + ? WHERE accountNum=?", [amount, account], (err) => {
        if (err) {
            console.error(err)
        } else {
            console.log("Receiver Update Successful")
        }
    })
    db.run("UPDATE users SET balance = balance - ? WHERE uid=?", [amount, req.session.num], (err) => {
        if (err) {
            console.error(err)
        } else {
            console.log("Sender Update Successful")
        }
    })
    res.redirect("/")
})

app.get('/login', (req, res) => {
    res.render('login')
})

app.post('/login', (req, res) => {
    let username = req.body.username
    let password = req.body.password
    db.get("SELECT * FROM users WHERE name=? AND password=?", [username, password], (err, row) => {
        if (row) {
            req.session.num = row.uid
            res.redirect("/")
        } else {
            console.error(err)
        }
    })
})

app.get('/logout', (req, res) => {
    req.session.destroy()
    res.redirect('/')
})

app.listen(400, (err) => {
    if (err) {
        console.error(err)
    } else {
        console.log('Server running on port 400')
    }
})