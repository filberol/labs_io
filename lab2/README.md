# lsblk
NAME                      MAJ:MIN RM   SIZE RO TYPE MOUNTPOINTS
vramdisk                  252:0    0    50M  0 disk
├─vramdisk1               252:1    0    10M  0 part
├─vramdisk2               252:2    0     1K  0 part
├─vramdisk5               252:5    0     5M  0 part
└─vramdisk6               252:6    0    15M  0 part 

# fdisk -l /dev/vramdisk
Disk /dev/vramdisk: 50 MiB, 52428800 bytes, 102400 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x36e5756d

Device         Boot Start   End Sectors Size Id Type
/dev/vramdisk1          1 20479   20479  10M 83 Linux
/dev/vramdisk2      20480 61439   40960  20M  5 Extended
/dev/vramdisk5      20481 30719   10239   5M 83 Linux
/dev/vramdisk6      40961 71679   30719  15M 83 Linux

# Создаём файловые системы в дисках:
sudo mkfs.vfat /dev/vramdisk1
sudo mkfs.vfat /dev/vramdisk5
sudo mkfs.vfat /dev/vramdisk6

# Монтируем их:
sudo mkdir -p /mnt/vram1 && sudo mount -t vfat /dev/vramdisk1 /mnt/vram1
sudo mkdir -p /mnt/vram5 && sudo mount -t vfat /dev/vramdisk5 /mnt/vram5
sudo mkdir -p /mnt/vram6 && sudo mount -t vfat /dev/vramdisk6 /mnt/vram6

