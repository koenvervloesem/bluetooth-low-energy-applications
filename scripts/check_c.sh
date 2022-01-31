#!/usr/bin/env bash
DIR=$(pwd)

for directory in 3-advertisements 4-connections 5-security 6-profiles 7-lowpower 8-reverse; do
  cd "$DIR" || exit
  cd "$directory" || exit
  echo Checking "$directory"...

  if [ -d arduino ]; then
    echo Checking Arduino code...
    cd arduino
    for i in $(ls); do
      cd "$i" || exit
      echo Checking "$i"...
      make lint
      cd ..
    done
    cd ..
  fi

  if [ -d zephyr ]; then
    echo Checking Zephyr code...
    cd zephyr
    for i in $(ls); do
      cd "$i" || exit
      echo Checking "$i"...
      make lint
      cd ..
    done
    cd ..
  fi

done
