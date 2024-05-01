Attempt to reproduce the issue reported in HELP-58977.

Start a replica set with five nodes:
```bash
mlaunch init \
    --replicaset \
    --nodes=5 \
    --binarypath $HOME/bin/mongodl/archive/6.0.9/mongodb-macos-aarch64-enterprise-6.0.9/bin \
    --dir .menv
```

Build and run the example:
```bash
./build.sh
./run.sh
```

Kill a mongod:
```bash
./kill-one-mongod.sh
```

To restart the test, start the killed process again:
```bash
mlaunch start --dir .menv
```
