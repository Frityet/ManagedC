option("ansi")
do
    set_default(false)
    set_showmenu(true)

    set_description("Compiles tests with ANSI-C89 standards, which disables the mc_auto and mc_defer tests")
end
option_end()

option("thread-sanitizer")
do
    set_default(false)
    set_showmenu(true)

    set_description("Compiles tests with the thread sanitizer, instead of the regular address,leak,undefined")
end
option_end()

local CFLAGS = {
    "-Wall", "-Wextra", "-Werror",
    "-Weverything",
    "-Wno-unused-parameter", "-Wno-unused-variable", "-Wno-unused-function", "-Wno-unused-macros",
    "-Wno-missing-variable-declarations",
    "-Wno-keyword-macro",
    "-Wno-reserved-identifier",
    "-Wno-comma",
    "-Wno-cast-qual",
    "-Wno-bad-function-cast",
    "-Wno-atomic-implicit-seq-cst",
    "-Wno-declaration-after-statement",
}

local SANITIZERS = {}

if has_config("ansi") then
    add_cflags {
        "-ansi",
        "-Wpedantic"
    }

    set_languages("c89")
    add_defines("__STRICT_ANSI__")
else
    CFLAGS[#CFLAGS + 1] = "-Wno-gnu-statement-expression"
    CFLAGS[#CFLAGS + 1] = "-Wno-nullability-extension"
    CFLAGS[#CFLAGS + 1] = "-Wno-vla"

    add_undefines("__STRICT_ANSI__")
    set_languages("gnulatest")
end

if has_config("thread-sanitizer") then
    SANITIZERS[#SANITIZERS + 1] = "thread"
else
    SANITIZERS[#SANITIZERS + 1] = "address"
    -- SANITIZERS[#SANITIZERS + 1] = "leak"
    SANITIZERS[#SANITIZERS + 1] = "undefined"
end

add_rules("mode.debug", "mode.release")


target("tests")
do
    set_kind("binary")
    add_files("src/**.c")

    add_cflags(CFLAGS)

    add_options("ansi")
    if is_mode("debug") then
        add_cflags("-fno-omit-frame-pointer")
        for i, v in ipairs(SANITIZERS) do
            add_cflags("-fsanitize=" .. v)
            add_ldflags("-fsanitize=" .. v)
        end
    end

    add_deps("ManagedC")
end
target_end()
