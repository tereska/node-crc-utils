Node CRC32 Utils
==============

Combines two or more CRC32 checksums into new one.

##How to build/install:
```
node-gyp configure build
```
or
```
npm install crc-utils
```

##Example:
```
var crcUtils = require('crc-utils');

// for crc32 checksum use lib: https://github.com/brianloveswords/buffer-crc32/
var crc32 = require('buffer-crc32');

var foo = new Buffer('foo');
var bar = new Buffer('bar');

var fooCrc32 = crc32(foo); // <Buffer 8c 73 65 21>
var barCrc32 = crc32(bar); // <Buffer 76 ff 8c aa>
 
var foobar = new Buffer('foobar');
var foobarCrc32 = crc32(foobar);

var foobarCrc32Combined = crcUtils.crc32_combine(
  fooCrc32.readUInt32BE(0), 
  barCrc32.readUInt32BE(0), 
  bar.length
); 

// CRC32 are the same but Endianness is prepared for GZIP format
console.log(foobarCrc32);         // <Buffer 9e f6 1f 95>
console.log(foobarCrc32Combined); // <Buffer 95 1f f6 9e>
```
