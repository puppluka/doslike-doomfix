:: use when versioning updates
set VERSION="0.4.1.0"

set APP_TITLE="\"AexDoom\""
..\tcc\tcc -E ..\source\doom.c ..\source\dos.c -DDISABLE_SYSTEM_CURSOR -DSECRETS_ENABLED -DWINDOW_APPNAME=%APP_TITLE% -o doom_tree.c