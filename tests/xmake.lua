local CFLAGS = {
    "-Wall", "-Wextra", "-Werror", 
    "-Weverything",
    sanitizers = "address,leak,undefined",
    "-Wno-unused-parameter", "-Wno-unused-variable", "-Wno-unused-function", "-Wno-unused-macros",
    "-Wno-missing-variable-declarations",
    "-Wno-keyword-macro",
    "-Wno-reserved-identifier",
    "-Wno-comma",
    "-Wno-cast-qual",
    "-Wno-bad-function-cast",
    "-Wno-atomic-implicit-seq-cst",
    "-Wno-declaration-after-statement",
    "-Wno-gnu-statement-expression",
    "-Wno-nullability-extension"
}

option("ansi")
do
    set_default(false)
    set_showmenu(true)

    add_cflags {
        "-ansi",
        "-Wpedantic"
    }

    set_languages("c89")
    add_defines("__STRICT_ANSI__")
end
option_end()

add_rules("mode.debug", "mode.release")

set_languages("gnu11")

target("tests")
do
    set_kind("binary")
    add_files("src/**.c")

    add_cflags(CFLAGS)

    add_options("ansi")
    if is_mode("debug") then
        add_cflags("-fsanitize=" .. CFLAGS.sanitizers, "-fno-omit-frame-pointer")
        add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)
    end

    add_deps("ManagedC")
end
target_end()
