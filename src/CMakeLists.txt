# select the 3D backend for sokol_gfx.h and sokol_app.h
# the sokol_config() macro is in the top-level CMakeLists.txt file!
sokol_config()

fips_ide_group(Samples/main)
fips_begin_app(main windowed)
    fips_vs_warning_level(3)
    fips_files(main.c)
    sokol_shader(main.glsl ${slang})
    fips_deps(sokol mpack cimgui)
fips_end_app()
