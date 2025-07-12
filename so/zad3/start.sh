#!/bin/bash

qemu-img create -f qcow2 -o backing_file=backup_minix.img minix.img

qemu-system-x86_64 -curses -drive file=minix.img -enable-kvm -localtime -net user,hostfwd=tcp::10022-:22 -net nic,model=virtio -m 2048M

# ssh-copy-id root@localhost -p 10022

# ssh alex@10.0.2.2 -- do polaczen z localhostem z minixa
