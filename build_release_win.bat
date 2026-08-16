@echo off

scons platform=windows target=template_release debug_symbols=yes profiler=tracy profiler_track_memory=yes profiler_record_on_demand=no profiler_path=C:\Projects\GDGameOne\tracy prodution=yes disable_physics_2d=yes disable_navigation_2d=yes module_svg_enabled=no module_gltf_enabled=no
