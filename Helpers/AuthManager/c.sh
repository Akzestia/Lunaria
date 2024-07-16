#!/bin/bash

g++ -o auth_manager main.cpp AuthManager.cpp -lssl -lcrypto

./auth_manager