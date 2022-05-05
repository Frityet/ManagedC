add_rules("mode.release", "mode.debug")

includes("../package.lua")

local CFLAGS<const> = {
    "-Wall", "-Wextra", "-Werror",
    sanitizers = "address,leak,undefined"
}

local DEPENDENCIES<const> = {
    "managedc",
    "libuv"
}

add_requires(DEPENDENCIES)

target("Example")
do
    set_kind("binary")
    add_files("src/**.c")
    add_headerfiles("src/**.h")
    add_includedirs("src/", "src/include/")
    add_cflags(CFLAGS, "-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
    add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)

    add_packages(DEPENDENCIES)
end
target_end()
