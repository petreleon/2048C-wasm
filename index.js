var factory = require('./api.js');
console.reset = function () {
  return process.stdout.write('\033c');
}
factory().then((instance) => {
  instance._init();
  instance._read();
  var stdin = process.stdin;
  stdin.setRawMode(true);
  stdin.resume();
  stdin.setEncoding('utf8');

  stdin.on('data', function (key) {
    if (key == '\u001B\u005B\u0041') {
      instance._move('u'.charCodeAt(0));
      //process.stdout.write('up'); 
    }
    if (key == '\u001B\u005B\u0043') {
      instance._move('r'.charCodeAt(0));
      //process.stdout.write('right'); 
    }
    if (key == '\u001B\u005B\u0042') {
      instance._move('d'.charCodeAt(0));
      //process.stdout.write('down'); 
    }
    if (key == '\u001B\u005B\u0044') {
      instance._move('l'.charCodeAt(0));
      //process.stdout.write('left'); 
    }
    console.reset();
    instance._read();
    if (key == '\u0003' || instance._movable() == 0) { process.exit(); }    // ctrl-c

  });

});