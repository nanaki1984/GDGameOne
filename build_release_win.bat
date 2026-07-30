@echo off

scons platform=windows target=template_release debug_symbols=yes d3d12=yes profiler=tracy profiler_track_memory=yes profiler_record_on_demand=no profiler_path=C:\Projects\GDGameOne\tracy prodution=yes accesskit=no angle=no
