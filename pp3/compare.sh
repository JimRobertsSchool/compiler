#!/bin/bash

make -s clean
make -s

mkdir tocompare
cp samples/*.out tocompare/

cd tocompare
ln -s ../dcc ./dcc

cd ../tocompare

for each in *.out; 
do
	filename=$(basename "$each")
	extension="${filename##*.}"
	filename="${filename%.*}"
	cp ../samples/"$filename".decaf ./
done

ERRORS=0

for each in *.out;
do
	filename=$(basename "$each")
	extension="${filename##*.}"
	filename="${filename%.*}"
	fin="$filename".decaf
	file1="$each"
	file2="$filename".mine
	./dcc < $fin &> "$file2"
	sort "$file2" -o "$file2"
	sort "$file1" -o  "$file1"
	output=$(diff -q -w "$file1" "$file2")
	d=`diff -w "$file1" "$file2"`
	echo "$fin"

	if [ -n "$output" ]; then
		echo "$file1"
		echo "$d"
		read
		ERRORS=1
	fi
done

if [ "$ERRORS" == "0" ];
then
	>&2 echo "No differences detected, try adding more test cases to ./samples"
fi


cd ../

rm -rf tocompare
make -s clean
