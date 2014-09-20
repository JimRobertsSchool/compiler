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

ERRORS=0

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
		$ERRORS=1
	fi
done

if [ "$ERRORS" == "0" ];
then
	>&2 echo "No differences detected, try adding more test cases to ./samples"
fi


cd ../

rm -rf tocompare
make -s clean
