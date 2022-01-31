#!/usr/bin/env bash
BASE_DIR="$1"

for directory in 3-advertisements 4-connections 5-security 6-profiles 7-lowpower 8-reverse; do
  echo Checking "$directory"...
  ZEPHYR_DIR=$BASE_DIR/"$directory"/zephyr
  if [ -d $ZEPHYR_DIR ]; then
    echo Building Zephyr code...
    for i in $(ls $ZEPHYR_DIR); do
      echo Building "$i"...
      west build -p auto -b nrf52840dk_nrf52840 -s $ZEPHYR_DIR/"$i"
    done
  fi

done
