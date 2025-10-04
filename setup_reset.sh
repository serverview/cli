#!/bin/bash

echo "Resetting setup..."

# Remove directories
rm -rf /var/www/svh
rm -rf /etc/serverview
rm -rf /var/log/serverview
rm -rf /var/serverview

# Remove core file
rm -f /usr/local/sbin/svcore

echo "Setup reset complete."