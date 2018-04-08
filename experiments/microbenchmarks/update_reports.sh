#!/bin/sh

for f in results/*.txt
do
    SRC=$f
    TRG="${SRC%.txt}".rst
    echo "(re)creating $TRG"
    python report.py $SRC "^#" > $TRG
done
