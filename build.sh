#!/bin/bash
gcc -o dist/svctl $(find src -name "*.c") -lcurl