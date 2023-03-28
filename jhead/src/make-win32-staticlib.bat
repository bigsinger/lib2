if not exist obj md obj
nmake -f makefile-win32-staticlib DEBUG=1
nmake -f makefile-win32-staticlib