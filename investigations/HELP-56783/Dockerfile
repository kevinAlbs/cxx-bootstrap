# Dockerfile
FROM mongodb/mongo-cxx-driver:3.10.0-redhat-ubi-9.3

WORKDIR /build

RUN microdnf upgrade -y && microdnf install -y g++

COPY repro.cpp /build/

RUN g++ \
    -o repro \
    repro.cpp \
    -I/usr/local/include/bsoncxx/v_noabi/ \
    -I/usr/local/include/mongocxx/v_noabi/ \
    -lmongocxx \
    -lbsoncxx

CMD /build/repro
