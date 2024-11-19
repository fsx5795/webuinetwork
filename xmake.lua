if is_host("windows") then
    add_requires("openssl")
end
set_languages("c23", "c++23")
target("webui")
    after_build(function (target)
        import("core.project.task")
        task.run("project", {kind = "compile_commands", outputdir = "./build"})
    end)
    add_files("src/*.cpp")
    add_includedirs("include")
    if is_mode("debug") then
        add_linkdirs("lib/debug")
    else
        add_linkdirs("lib/release")
    end
    if is_host("windows") then
        add_packages("openssl")
        add_links("webui-2-secure-static", "user32", "advapi32", "shell32", "ole32")
    else
        add_links("webui-2-static")
    end
    --add_packages("Threads")