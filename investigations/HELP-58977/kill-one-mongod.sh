#!/usr/bin/env bash

# List starting number of mongods.
num_mongod_start=$(pgrep mongod | wc -l)
echo "There are $num_mongod_start mongod running"

# Kill one mongod.
pid=$(pgrep mongod | head -n 1)
echo "Killing mongod at PID $pid ..."
kill $pid

# Wait for mongod to terminate.
while true; do
    if kill -0 "$pid" >/dev/null 2>&1; then
        echo "mongod with PID $pid is still running. Sleeping for 1 second"
        sleep 1
    else
        echo "mongod with PID $pid is not running. Exiting"
        break
    fi
done
