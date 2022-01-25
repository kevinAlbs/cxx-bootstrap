import pymongo

client = pymongo.MongoClient("mongodb+srv://bootstrapmongodb.4lwdw.mongodb.net/myFirstDatabase?authSource=%24external&authMechanism=MONGODB-X509&tlsCertificateKeyFile=/Users/kevin.albertson/.secrets/help30888/X509-cert-5915913444876671085.pem")
print (client["db"].command({"ping": 1}))