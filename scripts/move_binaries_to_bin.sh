#!/bin/bash
BASEDIR=$(dirname $0)

#helping tools
cp "${BASEDIR}/../tools/receive_UDP/Release/receive_UDP" "${BASEDIR}/../bin"
cp "${BASEDIR}/../tools/send_UDP/Release/send_UDP" "${BASEDIR}/../bin"

#streamsaver program
cp "${BASEDIR}/../Release/streamsaver" "${BASEDIR}/../bin"

