#!/bin/bash

if [ ! -d bin ]; then
	mkdir bin
fi

if [ ! -d obj ]; then
	mkdir obj
fi

if [ ! -d example ]; then
	mkdir example
fi

make

# build open source tool to generate fru.bin
cd tool/ipmi-fru-it
make
cd -

./tool/ipmi-fru-it/ipmi-fru-it -s 8192 -c tool/fru.conf -a -o tool/fru.bin



