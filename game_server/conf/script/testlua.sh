#!/bin/bash

##################################################
##简单测试lua文件有没有明显的错误
##sh testlua.sh [path]
##[path]默认为"./"
##################################################

filedir=""
if [ $# -eq 0 ]; then
	filedir="./"
else
	for files in $@; do
		filedir+=$files" "
	done
fi

luafiles=`find $filedir -regex ".*.lua"`

for file in $luafiles; do
	echo "execute "$file
	lua $file
done
