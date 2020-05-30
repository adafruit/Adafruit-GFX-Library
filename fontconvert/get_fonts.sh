#!/bin/bash

FREEFONTS_ZIP="$1"
FREEFONTS_VERSION=20120503
FREEFONTS_URL_BASE="http://ftp.gnu.org/gnu/freefont"

if [[ ! -f ${FREEFONTS_ZIP} ]]
then
  curl --output "${FREEFONTS_ZIP}" "${FREEFONTS_URL_BASE}/${FREEFONTS_ZIP}"
fi
