FILE=bw386389.patch
cp $FILE /usr/$FILE 
cd /usr/src
patch -p1 < $FILE 

cp /usr/src/minix/include/minix/com.h /usr/include/minix/com.h
cp /usr/src/minix/include/minix/callnr.h /usr/include/minix/callnr.h
cp /usr/src/include/unistd.h /usr/include/unistd.h
cp /usr/src/minix/include/minix/syslib.h /usr/include/minix/syslib.h

make_install() {
	where=$1

	cd ${where}
	make
	make install
}

make_install /usr/src/minix/servers/pm

make_install /usr/src/minix/servers/sched

make_install /usr/src/lib/libc

make_install /usr/src/lib

make_install /usr/src/minix/libc/libsys

cd /usr/src/releasetools
make do-hdboot
reboot
