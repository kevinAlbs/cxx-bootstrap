Start a mongod locally. Run this from the repository root directory:

```
mongod --dbpath ./tmp --tlsCAFile ./x509gen/ca.pem --tlsCertificateKeyFile ./x509gen/server.pem --tlsMode requireTLS --auth
```

Create a user in the shell with:
```
mongosh --tlsCAFile ./x509gen/ca.pem --tls --tlsCertificateKeyFile ./x509gen/client.pem
> db.createUser({user: "username", pwd: "password", roles: ["root"]})
```

Then, attempt to run repro. Run this from the cxx2388 directory:

```
MONGODB_URI='mongodb://username:password@localhost:27017/admin?appName=Genny&maxPoolSize=2000&socketTimeoutMS=-1&ssl=true&tlsAllowInvalidHostnames=true&tlsCAFile=../../x509gen/ca.pem&tlsCertificateKeyFile=../../x509gen/client.pem' ./run.sh
```