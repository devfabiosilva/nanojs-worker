{
  "targets": [
    {
      "target_name": "nanojspow",
      "sources": [ "./src/nanojsworker.c", "./src/util.c" ],
      "include_dirs":[ "./include", "./include/sodium" ],
      "libraries": ["../lib/libnanocrypto1.a", "../lib/libsodium.a"],
    }
  ]
}

