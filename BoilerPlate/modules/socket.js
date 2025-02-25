function connection(socket) {
    socket.emit('chat message', `Welcome to the chat room ${socket.request.session.user}`);
    socket.on('chat message', (msg) => {
        console.log(`message: ${msg}`);
    });
    socket.on('disconnect', () => {
        console.log('User disconnected');
    });
}

module.exports = {
    connection
};