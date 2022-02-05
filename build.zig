//https://www.reddit.com/r/Zig/comments/kf8wgq/cmake_based_library/

const Builder = @import("std").build.Builder;

    pub fn build(b: *Builder) !void {

        const mode = b.standardReleaseOptions();
        const target = b.standardTargetOptions(.{});

        const efsw_prebuild = b.addSystemCommand(&[_][]const u8{
            "cmake",
            "-B",
            "extern/efsw/build",
            "-S",
            "extern/efsw",
            "-DCMAKE_BUILD_TYPE=Release"
        });
        try efsw_prebuild.step.make();
        const efsw_build = b.addSystemCommand(&[_][]const u8{
            "cmake",
            "--build",
            "extern/efsw/build",
            "-j"
        });
        try efsw_build.step.make();

        const exe = b.addExecutable("filewatch", null);
        exe.setBuildMode(mode);
        exe.setTarget(target);
        exe.linkLibC();
        exe.addIncludeDir("extern/efsw/include/");
        exe.addLibPath("extern/efsw/build/lib");
        exe.linkSystemLibrary("efsw");
        exe.addCSourceFiles(&.{"src/main.cpp"},
        &.{
            "-std=c++17",
            "-pedantic",
            "-Wall",
            "-W",
            "-Wno-missing-field-initializers",
            "-fno-sanitize=undefined",
        });
        exe.install();
        // ....
    }