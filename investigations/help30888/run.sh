export PATH=$PATH:/home/Administrator/code/cxx-bootstrap/install/mongo-c-driver-1.20.1/bin
export PATH=$PATH:/home/Administrator/code/cxx-bootstrap/install/mongo-cxx-driver-r3.6.6/bin

export CA_FILE="C:/cygwin/home/Administrator/code/drivers-evergreen-tools/.evergreen/x509gen/ca.pem"
# Case 1: PKCS#1 key.
# export CLIENT_FILE="C:/cygwin/home/Administrator/code/drivers-evergreen-tools/.evergreen/x509gen/client.pem"
# Case 2: PKCS#8 key.
export CLIENT_FILE="C:/cygwin/home/Administrator/code/drivers-evergreen-tools/.evergreen/x509gen/client-pkcs8-unencrypted.pem"
export MONGODB_URI="mongodb://localhost:27018/?tls=true&tlsCAFile=$CA_FILE&tlsCertificateKeyFile=$CLIENT_FILE"
./cmake-build/Debug/crepro.out

# For Case 2, got this output
# $ ./run.sh
# 2022/01/25 23:59:03.0700: [ 9916]:    ERROR: stream-secure-channel: Failed to parse private key. ASN1 bad tag value met.
# (0x8009310B)
# 2022/01/25 23:59:03.0930: [ 9916]:    ERROR: stream-secure-channel: Failed to parse private key. ASN1 bad tag value met.
# (0x8009310B)
# 2022/01/25 23:59:03.0954: [ 9916]:    ERROR:       mongoc: mongoc_collection_count_documents error: No suitable servers found (`serverSelectionTryOnce` set): [Failed to receive length header from server. calling hello on 'localhost:27018']