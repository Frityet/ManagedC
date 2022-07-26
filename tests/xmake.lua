add_rules("mode.debug", "mode.release")

local CFLAGS = {
    "-Wall", "-Wextra", "-Werror", "-Wpedantic",
    sanitizers = "address,leak,undefined",
    "-Wno-unused-parameter", "-Wno-unused-variable", "-Wno-unused-function", "-Wno-unused-but-set-variable",
}

set_languages("c89")
-- set_languages("gnulatest")

target("tests")
do
    set_kind("binary")
    add_files("src/**.c")
    add_cflags(CFLAGS, "-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
    add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)

    

    add_deps("ManagedC")
end
target_end()
