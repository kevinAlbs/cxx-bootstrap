To attempt to reproduce:

I created a user on a personal M0 instance.
- Selected "Authentication Method" of "Certificate".
- Used "Common Name" of "tsd-atlas-connectivity-test-user".
- Selected "Download certificate when user is added"
- Used "3 months" for "Select the certificate expiration for this user"

I am able to connect with mongosh:
mongosh "mongodb+srv://bootstrapmongodb.4lwdw.mongodb.net/myFirstDatabase?authSource=%24external&authMechanism=MONGODB-X509" --tls --tlsCertificateKeyFile "/Users/kevin.albertson/.secrets/help30888/X509-cert-5915913444876671085.pem"

I am able to connect with pymongo:

```python
import pymongo

client = pymongo.MongoClient("mongodb+srv://bootstrapmongodb.4lwdw.mongodb.net/myFirstDatabase?authSource=%24external&authMechanism=MONGODB-X509&tlsCertificateKeyFile=/Users/kevin.albertson/.secrets/help30888/X509-cert-5915913444876671085.pem")
print (client["db"].command({"ping": 1}))
```

I am able to connect with a C++ example on macOS. See main.cpp.

One oddity: I am able to connect when specifying an incorrect username in the connection string.

Q: Does the common name (CN) in the client certificate match the username in the connection string?
This can be determined with the openssl CLI. For example:
{noformat}
openssl x509 -subject -in ~/.secrets/help30888/X509-cert-5915913444876671085.pem -noout
subject= /CN=tsd-atlas-connectivity-test-user
{noformat}

One workaround is to convert the private key from PKCS#8 format to PKCS#1 as follows:

{noformat}
openssl x509 -in X509-cert-5915913444876671085.pem -out certificate.pem
openssl rsa -in X509-cert-5915913444876671085.pem -out private_key_pkcs1.pem
cat certificate.pem private_key_pkcs1.pem > client.pem
{noformat}

After doing this with the .pem file I downloaded from Atlas, I was able to authenticate with X509 using the C++ driver on Windows.