FILE=bw386389.patch
cp $FILE /$FILE 
cd /
patch -p1 < $FILE 

cd /usr/src/minix/servers/pm
make
make install

cd /usr/src/lib/libc
make
make install

cd /usr/src/releasetools
make do-hdboot
reboot

