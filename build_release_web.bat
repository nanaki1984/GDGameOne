@echo off

set EMSDK_QUIET=1
call ..\emsdk\emsdk_env.bat
scons platform=web target=template_release d3d12=no prodution=yes disable_physics_2d=yes disable_navigation_2d=yes module_svg_enabled=no module_gltf_enabled=no
