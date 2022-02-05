set /p value=< attendance_id
echo "connect with attendance id %value%"
bin/filewatch.exe -rv --post /attendances/%value%/file_snapshots --ping  /attendances/%value%/pings
