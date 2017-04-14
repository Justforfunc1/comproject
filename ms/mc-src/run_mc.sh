#!/bin/bash

APPDIR=/run/qpid_tools/bin/

${APPDIR}/qpid-config add queue ps.ms_1 --durable
${APPDIR}/qpid-config add queue ps.cc --durable
${APPDIR}/qpid-config add queue ps.mc --durable
#${APPDIR}/qpid-config add queue ps.ms_2 --durable

${APPDIR}/qpid-config add exchange fanout ps.ms.topic --durable

${APPDIR}/qpid-config bind ps.ms.topic ps.ms_1
#${APPDIR}/qpid-config bind ps.ms.topic ps.ms_2
