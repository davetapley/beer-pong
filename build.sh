#!/bin/bash
gcc `pkg-config --cflags --libs opencv` -lstdc++ -g -o bin/tracker tracker.cpp
