#!/usr/bin/env bash
set -e

DIR=$(pwd)

for directory in 3-advertisements 4-connections 5-security 6-profiles 7-lowpower 8-reverse; do
  cd "$DIR" || exit
  cd "$directory" || exit
  echo Checking "$directory"...

  if [ -d arduino ]; then
    echo Building Arduino code...
    cd arduino
    for i in $(ls); do
      cd "$i" || exit
      echo Building "$i"...
      make build
      cd ..
    done
    cd ..
  fi

done
