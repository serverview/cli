#!/bin/bash

echo "Resetting setup..."

# Stop and disable the systemd service
systemctl stop svcore
systemctl disable svcore
rm -f /etc/systemd/system/svcore.service
systemctl daemon-reload

# Remove directories
rm -rf /var/www/svh
rm -rf /etc/serverview
rm -rf /var/log/serverview
rm -rf /var/serverview
rm -rf /var/run/serverview

# Remove core file
rm -f /usr/local/sbin/svcore

echo "Setup reset complete."