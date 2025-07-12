#!/bin/sh

sudo chown -R bartek drive

for dir in $(ls drive/client);
do
    echo "Client ($dir)"
    ls -l drive/client/$dir/gcc
    sudo chmod +x drive/client/$dir/gcc
    size drive/client/$dir/gcc
    drive/client/$dir/gcc --version
done