SUMMARY = "SSD1306 I2C OLED kernel driver"
DESCRIPTION = "${SUMMARY}"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

SRC_URI = "file://Makefile \
           file://ssd1306-i2c.c \
           file://COPYING \
          "
          
S = "${WORKDIR}"

KERNEL_MODULE_AUTOLOAD += "ssd1306-i2c"

RPROVIDES:${PN} += "kernel-module-ssd1306-i2c"
