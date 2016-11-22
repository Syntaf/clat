mkdir /mnt/ramdisk
mount -t tmpfs -o size=10m tmpfs /mnt/ramdisk
cp dat1.txt /mnt/ramdisk/dat1.txt
cp dat2.txt /mnt/ramdisk/dat2.txt
make
./ramdisk_test1
./ramdisk_test2
umount /mnt/ramdisk
rm -rf /mnt/ramdisk
