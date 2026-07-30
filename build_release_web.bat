@echo off

set EMSDK_QUIET=1
call ..\emsdk\emsdk_env.bat
scons platform=web target=template_release prodution=yes accesskit=no angle=no
