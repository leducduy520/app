#!/bin/bash

gdb --batch -ex "run" -ex "bt" --args /root/app/install/bin/Myapp \
    --cert "/root/app/X509-cert.pem" \
    --db-uri "mongodb+srv://cluster0.24ewqox.mongodb.net/?authSource=%24external&authMechanism=MONGODB-X509" \
    --db-name "app" \
    --db-coll-name "module_app"
