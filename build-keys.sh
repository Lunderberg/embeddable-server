#!/bin/bash

set -euo pipefail

# Make a key
openssl genrsa -des3 -out server.key 1024

# Make a signing request
openssl req -new -key server.key -out server.csr

# Fulfill the signing request, with own key
openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt

# Make a Diffie-Hallman pair
openssl dhparam -out dh1024.pem 1024

# Merge files
cat server.key > server.pem
cat server.crt >> server.pem
cat dh1024.pem >> server.pem
rm -f server.key dh1024.pem server.csr
