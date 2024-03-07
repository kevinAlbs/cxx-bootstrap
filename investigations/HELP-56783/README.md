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
# List starting number of mongoses.
num_mongos_start=$(pgrep mongos | wc -l)
echo "There are $num_mongos_start mongos running"

# Kill one mongos.
pid=$(pgrep mongos | head -n 1)
echo "Killing mongos at PID $pid ..."
kill $pid

# Wait for mongos to terminate.
while true; do
    if kill -0 "$pid" >/dev/null 2>&1; then
        echo "mongos with PID $pid is still running. Sleeping for 1 second"
        sleep 1
    else
        echo "mongos with PID $pid is not running. Exiting"
        break
    fi
done
```
