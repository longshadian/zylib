#!/bin/bash

temp_path="./temp"
dest_path="../../src/proto"

if [ ! -d "$temp_path" ]; then  
	mkdir "$temp_path"  
fi

cp ../*.proto ./temp/
ls ./temp/*.proto |xargs -i sed -i 's/option/\/\/option/' {}

rm -f $dest_path/*.h
rm -f $dest_path/*.cc

cd ./temp
/home/cq/libs/protobuf-3.3.0/src/protoc *.proto -I./  --cpp_out=../../../src/proto

cd ..
rm -rf ./temp

