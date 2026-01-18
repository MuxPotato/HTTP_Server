#!/usr/bin/env bash
set -e # if anything returns error, we exit the script

make
./httpserver

