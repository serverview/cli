#!/bin/bash
find src -name "*.c" | xargs gcc -o dist/svctl
