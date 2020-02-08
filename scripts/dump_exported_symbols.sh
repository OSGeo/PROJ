#!/bin/sh

if objdump -TC "$1" | grep "elf64-x86-64">/dev/null; then
    COLUMN=62
#elif objdump -TC "$1" | grep "elf32-i386">/dev/null; then
#    COLUMN=46
else
    echo "Unsupported architecture"
    exit 1
fi

objdump -TC "$1" | grep " g    DF .text" | cut -b "${COLUMN}-" | grep -v "thunk to" | sed "s/internal_//" | grep -v "Java_" | sed "s/std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >/std::string/g" | sed "s/std::string >/std::string>/g" | sed "s/\[abi:cxx11\]//g" | sort -u
