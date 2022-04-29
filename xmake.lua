add_rules("mode.debug", "mode.release")



-- set_languages("gnu2x")
add_cflags("-std=gnu2x")

target("ManagedC")
do
    set_kind("headeronly")
    add_headerfiles("src/**.h")
    add_includedirs("src/", { public = true })

    -- add_cflags(CFLAGS)

    -- if is_mode("debug") then
    --     add_cflags("-fno-omit-frame-pointer")
    --     --add_cflags("-fsanitize=" .. CFLAGS.sanitizers)
    --     --add_ldflags("-fsanitize=" .. CFLAGS.sanitizers)
    -- end
end
target_end()

includes("tests")

