#!/usr/bin/env bash
DIR=$(pwd)

for directory in 3-advertisements 4-connections 5-security 6-profiles 7-lowpower 8-reverse; do
  cd "$DIR" || exit
  cd "$directory" || exit
  echo Checking "$directory"...

  if [ -d bleak ]; then
    echo Checking Python code...
    cd bleak
    make check
    cd ..
  fi
done
