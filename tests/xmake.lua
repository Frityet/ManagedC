add_rules("mode.debug")

local CFLAGS<const> = {
    "-Wall", "-Wextra", "-Werror",
    -- "-fmodules", "-fmodule-map-file=$(projectdir)/src/modules.modulemap",
    "-fms-extensions", "-Wno-microsoft-anon-tag",
    sanitizers = "leak,undefined"
}

target("Test")
do
    set_kind("binary")
    add_files("src/**.c")

    add_deps("ManagedC")
end
target_end()