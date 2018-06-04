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



