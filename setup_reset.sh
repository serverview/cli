#!/bin/bash

echo "Resetting setup..."

# Stop the default site process
if [ -f /var/run/serverview/default.pid ]; then
    kill $(cat /var/run/serverview/default.pid)
fi

# Remove directories
rm -rf /var/www/svh
rm -rf /etc/serverview
rm -rf /var/log/serverview
rm -rf /var/serverview
rm -rf /var/run/serverview

# Remove core file
rm -f /usr/local/sbin/svcore

echo "Setup reset complete."