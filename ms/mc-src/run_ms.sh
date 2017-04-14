#!/bin/bash


APPDIR=/run/qpid_tools/bin/

${APPDIR}/qpid-config add queue ps.cs_1 --file-count=10 --file-size=1024 --durable
${APPDIR}/qpid-config add queue ps.cs_4 --file-count=10 --file-size=1024 --durable
${APPDIR}/qpid-config add queue ps.ams_1_1 --file-count=10 --file-size=1024 --durable
${APPDIR}/qpid-config add queue ps.ams_1_2 --file-count=10 --file-size=1024 --durable
${APPDIR}/qpid-config add queue ps.ams_1_3 --file-count=10 --file-size=1024 --durable
${APPDIR}/qpid-config add queue ps.ams_1_4 --file-count=10 --file-size=1024 --durable
#${APPDIR}/qpid-config add queue ps.ams_1_5 --file-count=10 --file-size=1024 --durable
#${APPDIR}/qpid-config add queue ps.ams_1_6 --file-count=10 --file-size=1024 --durable
#${APPDIR}/qpid-config add queue ps.ams_1_7 --file-count=10 --file-size=1024 --durable
${APPDIR}/qpid-config add queue ps.ms --file-count=10 --file-size=1024 --durable

${APPDIR}/qpid-config add exchange fanout ps.ams.topic --durable

${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_1
${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_2
${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_3
${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_4
#${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_5
#${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_6
#${APPDIR}/qpid-config bind ps.ams.topic ps.ams_1_7

