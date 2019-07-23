#!/bin/bash

for ARG in "$@"
do
	tr -d "\r" < "$ARG" > "${ARG}.temp"
	rm "$ARG"
	mv "${ARG}.temp" "${ARG}"
done
