#!/bin/bash

if [ $# -ne 1 ];
then
    echo "Error: Expected name of the cpp file as parameter."
    echo "Usage: ./beautifier.sh [name]"
    echo "Example: ./beautifier.sh main.h (produces file main.h from mainX.h)"
    exit 0
fi

# https://stackoverflow.com/questions/965053/extract-filename-and-extension-in-bash
filename=$1
name="${filename%.*}"
extension=".${filename##*.}"

file=${name}X${extension}

if [ ! -f ${file} ];
then
    echo "Error: File $file doesn't exist"
    exit 0
fi


#echo $name __ $extension

#g++-7 ${file} -E -o ${name}2${extension}
g++ ${file} -E -o ${name}2${extension}

#clean from garbage
grep -A10000 "class lookup_error" ${name}2${extension} > ${name}3${extension}
grep -vwE "#" ${name}3${extension} > ${name}4${extension} #remove unnecesary "#"

#append includes and header guard
header=$(echo ${filename^^} | tr . _)
echo -e "#ifndef ${header}\n #define ${header} \n" >> ${name}6${extension}

grep -E "#include" ${file} >> ${name}6${extension}
echo -e "\n" >> ${name}6${extension}

echo "$(cat ${name}4${extension})" >> ${name}6${extension}
echo -e "\n #endif // ${header}" >> ${name}6${extension}

#clean from {};
clang-format -style=file ${name}6${extension} > ${name}7${extension}
grep -vwE "^\s+\{\};" ${name}7${extension}  > ${name}8${extension} #remove {}; empty blocks (only if it is the only thing in the line)

# add Header guards
clang-format -style=file ${name}8${extension} > ${name}${extension}

#clean main2
rm ${name}[0-9]${extension}
