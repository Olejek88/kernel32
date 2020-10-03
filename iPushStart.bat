@echo off
copy "\Compact Flash\IceTechnology\iPushCE\iPushCE.dll" \Windows
services stop HTP0:
services unload HTP0:
"\Compact Flash\tu\kernel32.exe"

