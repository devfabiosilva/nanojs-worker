const NANOJS_POW = require('../build/Release/nanojspow');

function main() {

   work = NANOJS_POW.nanojs_pow('81b13962a2e0f2b61a49ff0bb9a90c0ba6fb5bb1e99569c98659bb5b77c91772', 4)

   console.log(`Work = ${work.toString(16)} ${work.toString()}`);
   console.log('Hello World');
}

main();

