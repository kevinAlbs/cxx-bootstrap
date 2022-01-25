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

