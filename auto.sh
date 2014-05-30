#!/bin/bash
llvm_dir=/home/tk/Desktop/llvm-3.3.src
pass_dir=$llvm_dir/lib/Transforms/const_fol
bin_dir=$llvm_dir/Release+Asserts/bin
lib_dir=$llvm_dir/Release+Asserts/lib
include=$llvm_dir/include
libs="-L $lib_dir -lLLVMCore -lLLVMSupport -lpthread -ldl"
define="-D_DEBUG -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS"
#cc_conf=`$bin_dir/llvm-config --cppflags --ldflags --libs core`

# g++ kalei.cpp -I $include $define -L $lib_dir $libs -o ./tmp/kalei
# ./tmp/kalei < test.k

mkdir -p $pass_dir
cp ./pass/* $pass_dir
make -C $pass_dir
#exit

rm -rf tmp
mkdir -p tmp

if [ -e test.ll ]
then
	echo "apply pass..."
	cp test.ll ./tmp/test.ll
	$bin_dir/opt -load $lib_dir/libconstfold.so -const-fold -S -o ./tmp/test_opt.ll < ./tmp/test.ll 

	diff test.ll ./tmp/test_opt.ll
else
	echo "generating ll..."
	$bin_dir/clang -O0 -emit-llvm test.c -c -o ./tmp/test.bc
	$bin_dir/llvm-dis < ./tmp/test.bc > ./tmp/test.ll
fi
