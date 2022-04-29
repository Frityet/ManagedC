add_rules("mode.debug", "mode.release")

local CFLAGS<const> = {
    "-Wall", "-Wextra", "-Werror",
    -- "-fmodules", "-fmodule-map-file=$(projectdir)/src/modules.modulemap",
    "-fms-extensions", "-Wno-microsoft-anon-tag",
    sanitizers = "leak,undefined"
}

-- set_languages("gnu2x")
add_cflags("-std=gnu2x")


target("ManagedC")
do
    set_kind("binary")
    add_files("src/**.c")
    add_headerfiles("src/**.h")
    add_includedirs("src", "src/include/")

    add_cflags(CFLAGS)

    if is_mode("debug") then
        add_cflags("-fno-omit-frame-pointer")
        --add_cflags("-fsanitize=" .. CFLAGS.sanitizers)
        --add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)
    end
end
target_end()
