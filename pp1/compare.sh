#!/bin/bash

mkdir tocompare
cp samples/*.frag tocompare/
cp samples/*.decaf tocompare/

cd tocompare
ln -s ../dcc ./dcc
ln -s ../solution/dpp ./dpp

cd ../tocompare

for each in *.decaf *.frag; 
do
	cd ../solution
	./dcc < ../tocompare/$each > ../tocompare/"$each".out
	cd ../tocompare
done
cd ../

#rm -rf tocompare
