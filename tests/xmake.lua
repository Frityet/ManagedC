add_rules("mode.debug")

local CFLAGS<const> = {
    "-Wall", "-Wextra", "-Werror",
    sanitizers = "address,leak,undefined"
}

set_languages("gnu17")

target("Test")
do
    set_kind("binary")
    add_files("src/**.c")
    add_cflags(CFLAGS, "-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
    add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)

    add_deps("ManagedC")
end
target_end()
