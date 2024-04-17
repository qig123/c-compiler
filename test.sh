#!/bin/bash
assert(){
    expected="$1"
    input="$2"
    cc -o 9cc 9cc.c
    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 49 '5+6*7+2'
assert 15 '5*(9-6)'
assert 4 '(3+5)/2'
assert 10 '-10+20'
assert 10 '- -10'
assert 10 '- - +10'


echo OK