#!/bin/bash

#qemu-img create -f qcow2 -o backing_file=/home/students/inf/PUBLIC/SO/scenariusze/4/minix.img minix.img
#qemu-img create -f qcow2 -o backing_file=backup_minix.img minix.img

#Copy on write
#qemu-system-x86_64 -curses -drive file=minix.img -localtime -net user -net nic,model=virtio -m 1024M

#Bez copy on write
#qemu-system-x86_64 -curses -drive file=minix.img -enable-kvm -localtime -net user,hostfwd=tcp::10022-:22 -net nic,model=virtio -m 2048M
qemu-system-x86_64 -curses -drive file=minix_key.img -enable-kvm -localtime -net user,hostfwd=tcp::10022-:22 -net nic,model=virtio -m 2048M
