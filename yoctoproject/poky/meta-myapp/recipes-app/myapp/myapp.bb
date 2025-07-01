SUMMARY = "MyApp binary with haarcascade"
LICENSE = "CLOSED"
SRC_URI = "file://myapp \
           file://haarcascade_frontalface_default.xml"

S = "${WORKDIR}"

do_install() {
    # Cài binary vào /usr/bin/
    install -d ${D}${bindir}
    install -m 0755 ${WORKDIR}/myapp ${D}${bindir}/myapp

    # Cài haarcascade vào /usr/share/myapp/
    install -d ${D}${datadir}/myapp
    install -m 0644 ${WORKDIR}/haarcascade_frontalface_default.xml ${D}${datadir}/myapp/

    # Tạo sẵn thư mục ghi video: /var/lib/myapp/records
    install -d ${D}${localstatedir}/lib/myapp/records
}

FILES:${PN} += "${datadir}/myapp \
                ${localstatedir}/lib/myapp/records"

