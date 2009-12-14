cd ..\..\JISPParse
if exist %1.c erase /f %1.c
if exist temp.l erase temp.l
rename %1.l temp.l
echo %1.l
flex -otemp.c temp.l
rename temp.c %1.c
copy temp.l %1.l
erase temp.l