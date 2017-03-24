#!/bin/bash

temp_path="./temp"

if [ ! -d "$temp_path" ]; then  
	mkdir "$temp_path"  
fi

cp ../*.proto ./temp/
ls ./temp/*.proto |xargs -i sed -i 's/option/\/\/option/' {}

rm -f ../../src/proto/*.h
rm -f ../../src/proto/*.cc

cd ./temp
protoc *.proto --cpp_out=../../../src/proto/

cd ..
rm -rf ./temp
