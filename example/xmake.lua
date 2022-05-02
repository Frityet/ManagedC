add_rules("mode.release", "mode.debug")

includes("../package.lua")

local CFLAGS<const> = {
    "-Wall", "-Wextra", "-Werror",
    sanitizers = "leak,undefined"
}

local DEPENDENCIES<const> = {
    "managedc"
}

add_requires(DEPENDENCIES)



target("Example")
do
    set_kind("binary")
    add_files("src/**.c")
    add_cflags(CFLAGS, "-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
    add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)

    add_packages(DEPENDENCIES)
end
target_end()
