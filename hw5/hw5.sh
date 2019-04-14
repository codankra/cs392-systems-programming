#!/bin/bash
trap "exit" INT TERM ERR
trap "kill 0" EXIT
gcc server.c -o server
gcc client.c -o client
./server &
./client
wait