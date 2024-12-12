FROM ubuntu:latest

COPY node/ /node

# Define build-time secret as a certificate file
RUN --mount=type=secret,id=x509-cert,target=/tmp/X509-cert.pem \
    cp /tmp/X509-cert.pem /node/X509-cert.pem


