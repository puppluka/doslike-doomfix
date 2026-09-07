:: use when versioning updates
set VERSION="0.5.0.0"

set APP_TITLE="\"AexDoom %VERSION%-BETA\""
..\tcc\tcc -g ..\source\doom.c ..\source\dos.c ..\win32\resource.o -DDISABLE_SYSTEM_CURSOR -DSECRETS_ENABLED -DWINDOW_APPNAME=%APP_TITLE% -o ..\doom.exe