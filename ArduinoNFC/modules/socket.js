const main = require('./main.js');
const {Slammers} = require('./slammers.js');

function socketH(socket) {
    console.log('a user connected');
    socket.on('connect', function () {
        console.log('user connected');
    });

    socket.on('disconnect', function () {
        console.log('user disconnected');
    });

    socket.on('command', function (data) {
        const command = data[0];
        const record = data[1];
        const value = data.split('/')[1];

        if (command == 'w') {
            main.writeToPort(data, (dataValue) => {
                socket.emit('result', { requestedValue: "Writing was a success" });
            });
        }
        else if (command == 'r') {
            main.readFromPort(data, (dataValue) => {
                socket.emit('result', { requestedValue: dataValue });
            })

        }
        else {
            socket.emit('result', { requestedValue: 'Invalid command' });
        }
    })

    socket.on("search", function (data) {
        clientPdex = data;
    
        const dataValue = Slammers.find(slammer => slammer.pokedex == clientPdex);
    
        socket.emit('searchResult', { requestedValue: dataValue });
    });
}
module.exports = { socketH };