#
# Find the native LIBUSB includes and library
#
# This module defines
# LIBUSB_INCLUDE_DIR, where to find libusb.h
# LIBUSB_LIBRARY, the libraries to link against to use LIBUSB.
# LIBUSB_FOUND, If false, do not try to use LIBUSB.

FIND_PATH(LIBUSB_INCLUDE_DIR libusb.h
        PATHS
        /usr/local/include/libusb-1.0
        /usr/include/libusb-1.0
        /opt/local/include
        /opt/include
        )

get_filename_component(LIBUSB_INCLUDE_DIR ${LIBUSB_INCLUDE_DIR} ABSOLUTE)

FIND_LIBRARY(LIBUSB_LIBRARY usb-1.0
        /usr/local/lib
        /usr/lib
        )

IF (LIBUSB_INCLUDE_DIR)
    IF (LIBUSB_LIBRARY)
        SET(LIBUSB_FOUND "YES")
    ENDIF (LIBUSB_LIBRARY)
ENDIF (LIBUSB_INCLUDE_DIR)

MARK_AS_ADVANCED(
        LIBUSB_INCLUDE_DIR
        LIBUSB_LIBRARY
        LIBUSB_FOUND
)