add_rules("mode.debug", "mode.release")

target("ManagedC")
do
    set_kind("headeronly")
    add_headerfiles("src/**.h")
    add_includedirs("src/", { public = true })
end
target_end()

includes("tests")
