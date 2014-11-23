#! /bin/bash
gcc -o brightness brightness.c -framework IOKit \
	        -framework ApplicationServices -framework CoreFoundation
