#!/bin/sh
echo ; Generated at `date` by $0 >> uuids.txt
for i in `seq 0 20`;do(uuidgen >> uuids.txt);done;
