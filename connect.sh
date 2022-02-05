#!/bin/bash
value=$(head attendance_id -n 1)
echo "Connecting with attendance id: $value"
bin/filewatch -rv --post /attendances/$value/file_snapshots --ping /attendances/$value/pings
