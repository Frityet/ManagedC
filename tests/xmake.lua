local ANSI<const> = true

add_rules("mode.debug", "mode.release")

local CFLAGS = {
    "-Wall", "-Wextra", "-Werror", "-Weverything",
    sanitizers = "address,leak,undefined",
    "-Wno-unused-parameter", "-Wno-unused-variable", "-Wno-unused-function",
    "-Wno-missing-variable-declarations",
    "-Wno-keyword-macro",
    "-Wno-reserved-identifier",
    "-Wno-comma",
    "-Wno-cast-qual",
    "-Wno-bad-function-cast"
}

if ANSI then 
    CFLAGS[#CFLAGS + 1] = "-ansi"
    CFLAGS[#CFLAGS + 1] = "-Wpedantic"
end

if ANSI then 
    set_languages("c89")
    add_defines("__STRICT_ANSI__")
else
    set_languages("gnu11")
end

target("tests")
do
    set_kind("binary")
    add_files("src/**.c")

    add_cflags(CFLAGS, "-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
    add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)

    add_deps("ManagedC")
end
target_end()
