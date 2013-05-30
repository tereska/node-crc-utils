var crc = require('../');

// i'm using crc32 lib: https://github.com/brianloveswords/buffer-crc32/blob/master/index.js
var crc32 = require('buffer-crc32');

var foo = new Buffer('foo');
var bar = new Buffer('bar');

var fooCrc32 = crc32(foo); // <Buffer 8c 73 65 21>
var barCrc32 = crc32(bar); // <Buffer 76 ff 8c aa>
 
var foobar = new Buffer('foobar');
var foobarCrc32 = crc32(foobar);
var foobarCrc32Combined = crc.crc32_combine_multi([
  {crc: fooCrc32.readUInt32BE(0), len: foo.length},
  {crc: barCrc32.readUInt32BE(0), len: bar.length}
]); 


// Endianness prepared for GZIP format
console.log(foobarCrc32);         // <Buffer 9e f6 1f 95>
console.log(foobarCrc32Combined); // <Buffer 95 1f f6 9e>

