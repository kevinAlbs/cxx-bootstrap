#!/usr/bin/env bash
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
