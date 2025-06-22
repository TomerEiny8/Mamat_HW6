#!/bin/bash

# הפסקה אם יש שגיאת קומפילציה
set -e

echo "🛠️ מקמפל את הפרויקט..."

g++ -std=c++17 -Wall -Wextra -o test1_exec \
    L3.cpp L4.cpp test1.cpp

echo "🚀 מריץ את הבדיקות..."
./test1_exec
