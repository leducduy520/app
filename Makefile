linux-encode:
	base64 ./X509-cert.pem > mongo.pem.base64
linux-decode:
	base64 ./X509-cert.pem > mongo.pem
windows-encode:
	certutil -encode .\X509-cert.pem  mongo.pem.base64
windows-decode:
	certutil -decode .\mongo.pem.base64 mongo.pem
git-update:
	git fetch
	git pull
