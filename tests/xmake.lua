add_rules("mode.debug")

local CFLAGS<const> = {
    "-Wall", "-Wextra", "-Werror",
    sanitizers = ""--"leak,undefined"
}

target("Test")
do
    set_kind("binary")
    add_files("src/**.c")
    add_cflags(CFLAGS, "-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
    add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)

    add_deps("ManagedC")
end
target_end()
