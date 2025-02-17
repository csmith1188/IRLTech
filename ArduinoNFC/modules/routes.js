
const express = require('express');
const app = express();
PORT = 3000;
path = require('path');


function index(req, res) {
    res.render('index');
}

module.exports = {
    index,
}