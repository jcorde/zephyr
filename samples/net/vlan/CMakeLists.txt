cmake_minimum_required(VERSION 3.8.2)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(vlan)

target_sources(app PRIVATE src/main.c)

include($ENV{ZEPHYR_BASE}/samples/net/common/common.cmake)
