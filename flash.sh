#!/bin/bash
idf.py -p /dev/ttyUSB0 flash
screen /dev/ttyUSB0 115200
