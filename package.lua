package("managedc")
    set_kind("library", { headeronly = true })
    set_description("Reference counter garabage collection for C")

    add_urls("https://github.com/Frityet/ManagedC.git")
    add_versions("1.0.0", "350800021f8d41e5717e76f8dfb0485d45781996")
    add_versions("1.1.0", "878158a7f185aaaa38b8855afd91f89b05c9df13")

    on_install(function (package)
        local configs = {}

        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
         assert(package:has_cfuncs("alloc_managed", { includes = "managed.h" }))
    end)
