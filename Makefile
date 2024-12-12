linux-encode:
	base64 ./certs/X509-cert.pem > ./certs/X509-cert.pem.base64
linux-decode:
	base64 -d ./certs/X509-cert.pem.base64 > ./certs/X509-cert.pem
windows-encode:
	certutil -encode .\certs\X509-cert.pem .\certs\X509-cert.pem.base64
windows-decode:
	certutil -decode .\certs\X509-cert.pem.base64 .\certs\X509-cert.pem
git-update:
	git fetch
	git pull

# docker run --name apiservice -p 127.0.0.1:3000:3000 -e MONGODB_URI='mongodb+srv://cluster0.24ewqox.mongodb.net/?authSource=%24external&authMechanism=MONGODB-X509' -e MONGODB_CERT_ENCODE="$(echo "$(cat $(pwd)/certs/X509-cert.pem.base64)")" -w /node duyleduc520/apiservice
# gpg --symmetric --cipher-algo AES256 yourfile.pem
# gpg --output yourfile.pem --decrypt yourfile.pem.gpg
