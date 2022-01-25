SHELL := /usr/bin/env bash

BOARD = nrf52840dongle_nrf52840
SOURCE_FILES = src/*.c

.PHONY: build format lint

build:
	west build -b $(BOARD)

format:
	clang-format -i $(SOURCE_FILES)

lint:
	clang-format --dry-run $(SOURCE_FILES)
