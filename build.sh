#!/bin/bash
gcc `pkg-config --cflags --libs opencv` -lstdc++ -o bin/tracker tracker.cpp
