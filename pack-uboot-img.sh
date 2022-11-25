#!/bin/bash

sudo apt update
sudo apt install make imagemagick ttf-dejavu -y
if [ ! -f "gcc-linaro-arm-none-eabi-4.8-2014.04_linux.tar.xz" ];then
    curl -L -O https://redirect.armbian.com/_toolchain/gcc-linaro-arm-none-eabi-4.8-2014.04_linux.tar.xz
    tar xvf ./gcc-linaro-arm-none-eabi-4.8-2014.04_linux.tar.xz
    export PATH="$PATH:$(pwd)/gcc-linaro-arm-none-eabi-4.8-2014.04_linux/bin"
    echo "工具链下载完成"
else
    echo "工具链已存在"
fi

if [ ! -f "AmlImg" ];then
    ver="v0.3.0"
    curl -L -o ./AmlImg https://github.com/hzyitc/AmlImg/releases/download/$ver/AmlImg_${ver}_linux_amd64
    chmod +x ./AmlImg
else
    echo "AmlImg工具已存在"
fi

if [ -d build ];then
    cp build/* prebuild/
fi

if [ ! -d resource ];then
    mkdir -p resource
fi
convert \
-size 640x360 \
-background black \
-gravity center -font "DejaVu-Sans" -pointsize 64 -fill white label:"Hi NAS\nwww.ecoo.top" \
BMP3:resource/bootup.bmp

cat <<EOF >resource/list.txt
090000:bootup:bootup.bmp
EOF
./AmlImg res_pack prebuild/resource.img resource/

if [ ! -d pack ];then
    mkdir -p pack
fi

cp prebuild/ddr_init.bin pack/DDR.USB
cp prebuild/u-boot-comp.bin pack/UBOOT_COMP.USB

cat <<EOF >pack/aml_sdc_burn.ini
[common]
erase_bootloader    = 1
erase_flash         = 0
reboot              = 0

;package will filled by sdacard burning tool
[burn_ex]
package     = aml_upgrade_package.img
;media       =
EOF

cat <<EOF >pack/platform.conf
Platform:0x0801
BinPara:0xd9010000
DDRLoad:0xd9000000
DDRRun:0xd9000030
Uboot_down:0x200000
Uboot_decomp:0xd9000030
EOF

if [ -d build ];then
    cp build/* prebuild/
fi
cp prebuild/u-boot.bin pack/bootloader.img
echo -n "sha1sum $(sha1sum pack/bootloader.img | awk '{print $1}')" >pack/bootloader.VERIFY

cp prebuild/resource.img pack/resource.img
echo -n "sha1sum $(sha1sum pack/resource.img | awk '{print $1}')" >pack/resource.VERIFY

cat <<EOF >pack/commands.txt
USB:DDR:normal:DDR.USB
USB:UBOOT_COMP:normal:UBOOT_COMP.USB

ini:aml_sdc_burn:normal:aml_sdc_burn.ini
conf:platform:normal:platform.conf

PARTITION:bootloader:normal:bootloader.img
VERIFY:bootloader:normal:bootloader.VERIFY

PARTITION:resource:normal:resource.img
VERIFY:resource:normal:resource.VERIFY

# PARTITION:boot:normal:boot.img
# VERIFY:boot:normal:boot.VERIFY
# PARTITION:rootfs:normal:rootfs.img
# VERIFY:rootfs:normal:rootfs.VERIFY
EOF

./AmlImg pack prebuild/eMMC.burn.img pack/

