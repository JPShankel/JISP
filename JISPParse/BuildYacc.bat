cd ..\..\JISPParse

if exist %1.c erase /f %1.c
if exist %1.h erase /f %1.h
if exist %1.out.txt erase /f %1.out.txt
if exist temp.y erase temp.y
rename %1.y temp.y
echo %1.y
bison temp.y -d -v 
rename temp_tab.c %1.c
rename temp_tab.h %1.h
copy temp.y %1.y
rename temp.out %1.out.txt
erase temp.y