#!/bin/bash

find . -iname "*.pcat" -exec  rm {}."exe" \;
find . -iname "*.pcat" -exec  rm {}."code" \;
find . -iname "*.pcat" -exec  rm {}."frame" \;
find . -iname "*.pcat" -exec  rm {}."log" \;
find . -iname "*.pcat" -exec  rm {}."o" \;
find . -iname "*.pcat" -exec  rm {}."tree" \;
