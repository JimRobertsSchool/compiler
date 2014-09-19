#!/bin/bash

make -s clean
make -s

mkdir tocompare
cp samples/*.frag tocompare/
cp samples/*.decaf tocompare/

cd tocompare
ln -s ../dcc ./dcc
ln -s ../dpp ./dpp

cd ../tocompare

for each in *.decaf *.frag; 
do
	cd ../solution
	./dcc < ../tocompare/$each &> ../tocompare/"$each".out
	cd ../tocompare
done

for each in *.decaf *.frag;
do
	./dcc < $each &> ./"$each".mine
	file1="$each.mine"
	file2="$each.out"
	output=$(diff -q -w "$file1" "$file2")
	d=`diff -w "$file1" "$file2"`

	if [ -n "$output" ]; then
		echo "$file1"
		echo "$d"
		read
	fi
done


cd ../

rm -rf tocompare
make -s clean
