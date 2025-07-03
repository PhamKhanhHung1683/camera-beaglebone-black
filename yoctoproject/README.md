1. Follow Yocto Project documentation to clone Poky (krikstone)

2. Initialize the Build Environment
```bash
cd poky
source oe-init-build-env
```

3. Add Layer to the Layer Configuration File
```bash
cd poky/build
bitbake-layers add-layer ../meta-mykernel
bitbake-layers add-layer ../meta-myapp
```

4. Change the Configuration in poky/build/conf/local.conf
```bash
MACHINE ?= "beaglebone-yocto"

DL_DIR ?= "${TOPDIR}/../../sources/downloads"
SSTATE_DIR ?= "${TOPDIR}/../../sources/sstate-cache"

IMAGE_INSTALL:append = " kernel-module-ssd1306-i2c"

LICENSE_FLAGS_ACCEPTED += "commercial"
PACKAGECONFIG:append:pn-ffmpeg = " gpl x264"
IMAGE_INSTALL:append = " ffmpeg myapp openssh"
```