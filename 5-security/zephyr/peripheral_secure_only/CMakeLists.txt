# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(peripheral_secure_only)

target_sources(app PRIVATE src/service.c src/main.c)
