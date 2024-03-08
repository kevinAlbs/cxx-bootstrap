# Repeatedly stop and restart mongos processes.

while true; do
    echo "Starting killed mongos processes ..."
    # Restart killed mongos processes
    mlaunch start --dir .menv
    echo "Starting killed mongos processes ... done"

    # Kill one
    ./kill-one-mongos.sh

    # Kill another
    ./kill-one-mongos.sh

    echo "Waiting for 5 seconds"
done
