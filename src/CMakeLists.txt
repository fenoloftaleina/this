# select the 3D backend for sokol_gfx.h and sokol_app.h
# the sokol_config() macro is in the top-level CMakeLists.txt file!
sokol_config()

fips_ide_group(Samples/main)

fips_begin_sharedlib(main windowed)
    fips_vs_warning_level(3)
    fips_files(main.c)
    sokol_shader(main.glsl ${slang})
    fips_deps(sokol-dll mpack cimgui)
    fips_dir(data)
fips_end_sharedlib()

fips_begin_app(host windowed)
    fips_vs_warning_level(3)
    add_definitions(-DCR_DEPLOY_PATH="${FIPS_PROJECT_DEPLOY_DIR}")
    fips_files(host.cpp)
    fips_deps(sokol-dll)
fips_end_app()
