Attempt to reproduce the issue reported in HELP-56783.

Start a sharded cluster with 3 mongoses:
```bash
mlaunch init \
    --replicaset \
    --nodes=1 \
    --sharded=1 \
    --mongos=3 \
    --binarypath $HOME/bin/mongodl/archive/6.0.9/mongodb-macos-aarch64-enterprise-6.0.9/bin \
    --dir .menv
```

Build and run the example:
```bash
./build.sh
./run.sh
```

Kill a mongos:
```bash
./kill-one-mongos.sh
```

To restart the test, stop and start the sharded cluster processes:
```bash
mlaunch stop --dir .menv
mlaunch start --dir .menv
```
