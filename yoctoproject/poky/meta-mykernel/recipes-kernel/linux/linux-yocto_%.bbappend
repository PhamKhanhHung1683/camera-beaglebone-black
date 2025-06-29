FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI += " \
    file://defconfig \
    file://0001-add-ssd1306-i2c1.patch \
"

KCONFIG_MODE = "alldefconfig"
