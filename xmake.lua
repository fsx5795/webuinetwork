set_config("toolchain", "mingw")
set_config("mingw", "C:/Users/Nook/Videos/w64devkit")
set_languages("c23", "c++2a")
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
    if is_os("windows") then
        add_links("webui-2-static", "ws2_32", "ole32")
    else
        add_links("webui-2-static")
    end