var crc = require('../');
var zlib = require('zlib');

function createZFullFlush(userBuffer, cb){
  var buff = [];
  var gzip = zlib.createGzip({flush:zlib.Z_FULL_FLUSH});
  gzip.on('data', function(data) {
    buff.push(data);
  });
  gzip.on('end', function(){
    var concatBuff = Buffer.concat(buff);
    var len = concatBuff.length;
    cb({
      data: concatBuff.slice(10,-10), 
      len: concatBuff.readUInt32LE(len-4),
      crc: concatBuff.readUInt32LE(len-8)});
    });
  gzip.end(userBuffer);
}

var gzipHeader = new Buffer([0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03]);
var chunksEndBoundary = new Buffer([0x03, 0x00]);
var chunks = [];
createZFullFlush(new Buffer('slawek '), function(chunk1){
  createZFullFlush(new Buffer('janecki '), function(chunk2){
    createZFullFlush(new Buffer('rozwalil '), function(chunk3){
      createZFullFlush(new Buffer('gzipa!'), function(chunk4){
        chunks.push(chunk1, chunk2, chunk3, chunk4);
      });
    });
  });
});

require('http').createServer(function(req, res) {
    res.writeHead(200, { 'content-type':'text/plain; charset=UTF-8', 'content-encoding': 'gzip' });
    res.write(gzipHeader);
    for(var i=0,max=chunks.length;i<max;i++){
      res.write(chunks[i].data);
    }
    res.write(chunksEndBoundary);
    var data = crc.crc32_combine_multi(chunks);
    res.write(data.combinedCrc32);
    res.end(data.bufferLength);
}).listen(8888);



