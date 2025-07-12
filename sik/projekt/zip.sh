#!/bin/bash

FILES=$(cat .gitignore)
EXCLUDES=""

FILES+=' .git drive static tcp-test .vscode .gitignore'
FILES+=' docker Doxyfile'
FILES+=*.sh
for f in $FILES;
do
EXCLUDES+='--exclude='$f' '
done

cd ..
tar $EXCLUDES -zcvf bw386389.tar.gz bw386389
