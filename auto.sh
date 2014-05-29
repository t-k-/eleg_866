#!/bin/bash
llvm_dir=/home/tk/Desktop/llvm-3.3.src
pass_dir=$llvm_dir/lib/Transforms/const_fol
bin_dir=$llvm_dir/Release+Asserts/bin
lib_dir=$llvm_dir/Release+Asserts/lib

mkdir -p $pass_dir
cp ./pass/* $pass_dir
make -C $pass_dir

mkdir -p tmp
$bin_dir/clang -O0 -emit-llvm test.c -c -o ./tmp/test.bc
$bin_dir/opt -load $lib_dir/libconstfold.so -const-fold < ./tmp/test.bc > ./tmp/test_opt.bc

echo "result given by hello.bc:"
$bin_dir/lli ./tmp/test.bc 
echo "result given by hello_opt.bc:"
$bin_dir/lli ./tmp/test_opt.bc

$bin_dir/llvm-dis < ./tmp/test.bc > ./tmp/test.ll
$bin_dir/llvm-dis < ./tmp/test_opt.bc > ./tmp/test_opt.ll
