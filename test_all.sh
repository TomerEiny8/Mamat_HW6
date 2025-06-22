#!/bin/bash

# הפסקה אם יש שגיאת קומפילציה
set -e

echo "🛠️ מקמפל את הפרויקט..."

g++ -std=c++17 -Wall -Wextra -o test_exec \
    L3.cpp L4.cpp test.cpp

echo "🚀 מריץ את הבדיקות..."
./test_exec
