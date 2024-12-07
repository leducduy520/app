linux-encode:
	base64 ./certs/X509-cert.pem > X509-cert.pem.base64
linux-decode:
	base64 -d ./certs/X509-cert.pem.base64 > ./certs/X509-cert.pem
windows-encode:
	certutil -encode .\certs\X509-cert.pem X509-cert.pem.base64
windows-decode:
	certutil -decode .\certs\X509-cert.pem.base64 > .\certs\X509-cert.pem
git-update:
	git fetch
	git pull
