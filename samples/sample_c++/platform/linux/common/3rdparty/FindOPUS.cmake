#
# Find the native OPUS includes and library
#
# This module defines
# OPUS_INCLUDE_DIR, where to find opus.h
# OPUS_LIBRARY, the libraries to link against to use OPUS.
# OPUS_FOUND, If false, do not try to use OPUS.

FIND_PATH(OPUS_INCLUDE_DIR opus.h
        PATHS
        /usr/local/include/opus
        /usr/include/opus
        /opt/local/include
        /opt/include
        )

get_filename_component(OPUS_INCLUDE_DIR ${OPUS_INCLUDE_DIR} ABSOLUTE)

FIND_LIBRARY(OPUS_LIBRARY opus
        /usr/local/lib
        /usr/lib
        )

IF (OPUS_INCLUDE_DIR)
    IF (OPUS_LIBRARY)
        SET(OPUS_FOUND "YES")
    ENDIF (OPUS_LIBRARY)
ENDIF (OPUS_INCLUDE_DIR)

MARK_AS_ADVANCED(
        OPUS_INCLUDE_DIR
        OPUS_LIBRARY
        OPUS_FOUND
)