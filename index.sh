emcc helloworld.c -o api.js -s MODULARIZE -s EXPORTED_RUNTIME_METHODS=['ccall']
node index.js