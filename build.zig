const std = @import("std");

const engine_source_directories = [_][]const u8{ "src/core", "src/mods" };
const engine_extra_sources = [_][]const u8{
    "src/game/GameApplication.cpp",
    "src/game/GameSimulation.cpp",
};
const executable_sources = [_][]const u8{
    "src/main.cpp",
    "src/game/StaticGameRunner.cpp",
};
const test_source_directory = "tests";
const asset_directory = "assets";
const shader_directory = "src/core/render/shaders";

const cpp_flags = [_][]const u8{
    "-std=c++23",
    "-Wall",
    "-Wextra",
    "-Wpedantic",
    "-Wshadow",
    "-Wno-missing-field-initializers",
};

const opus_source_lists = [_]OpusSourceListProps{
    .{ .file_name = "celt_sources.mk", .variable_name = "CELT_SOURCES" },
    .{ .file_name = "silk_sources.mk", .variable_name = "SILK_SOURCES" },
    .{ .file_name = "silk_sources.mk", .variable_name = "SILK_SOURCES_FLOAT" },
    .{ .file_name = "opus_sources.mk", .variable_name = "OPUS_SOURCES" },
    .{ .file_name = "opus_sources.mk", .variable_name = "OPUS_SOURCES_FLOAT" },
};
const opus_include_directories = [_][]const u8{ "include", "celt", "silk", "silk/float", "src" };
const opus_c_flags = [_][]const u8{
    "-std=gnu99",
    "-DOPUS_BUILD",
    "-DVAR_ARRAYS",
    "-DHAVE_LRINT",
    "-DHAVE_LRINTF",
};

const windows_system_libraries = [_][]const u8{
    "d3dcompiler_47",
    "ws2_32",
    "userenv",
    "bcrypt",
    "ntdll",
    "opengl32",
    "propsys",
};

const macos_frameworks = [_][]const u8{
    "Metal",
    "QuartzCore",
    "Foundation",
    "CoreGraphics",
    "IOKit",
    "IOSurface",
};

const BuildContextProps = struct {
    builder: *std.Build,
    target: std.Build.ResolvedTarget,
    optimize: std.builtin.Optimize,
};

const ThirdPartyLibrariesProps = struct {
    sdl: *std.Build.Step.Compile,
    freetype: *std.Build.Step.Compile,
    opus: *std.Build.Step.Compile,
    stb_include_directory: std.Build.LazyPath,
    wgpu: ?*std.Build.Dependency,
};

const OpusSourceListProps = struct {
    file_name: []const u8,
    variable_name: []const u8,
};

pub fn build(b: *std.Build) !void {
    const context: BuildContextProps = .{
        .builder = b,
        .target = b.standardTargetOptions(.{}),
        .optimize = b.standardOptimizeOption(.{ .preferred_optimize_mode = .fast }),
    };

    const third_party_libraries = try resolveThirdPartyLibraries(context);
    const engine_library = try addEngineLibrary(context, third_party_libraries);

    const game_module = try createCppModule(context, &executable_sources);
    const game_executable = addLinkedExecutable(context, "wheel_of_fear", game_module, engine_library, third_party_libraries);
    b.installArtifact(game_executable);
    installRuntimeData(b);

    const run_game = b.addRunArtifact(game_executable);
    run_game.step.dependOn(b.getInstallStep());
    const run_step = b.step("run", "Build and run the game");
    run_step.dependOn(&run_game.step);

    const test_module = try createCppModule(context, try collectCppSources(b, test_source_directory));
    test_module.addIncludePath(b.dependency("doctest", .{}).path(""));
    test_module.addIncludePath(b.path(test_source_directory));
    const test_executable = addLinkedExecutable(context, "wheel_of_fear_tests", test_module, engine_library, third_party_libraries);

    const test_step = b.step("test", "Build and run the doctest suite");
    test_step.dependOn(&b.addRunArtifact(test_executable).step);
}

fn addLinkedExecutable(
    context: BuildContextProps,
    name: []const u8,
    module: *std.Build.Module,
    engine_library: *std.Build.Step.Compile,
    third_party_libraries: ThirdPartyLibrariesProps,
) *std.Build.Step.Compile {
    module.linkLibrary(engine_library);
    linkThirdPartyLibraries(module, third_party_libraries);
    linkWgpuNative(context, module, third_party_libraries);

    const executable = context.builder.addExecutable(.{
        .name = name,
        .root_module = module,
    });
    executable.bundle_ubsan_rt = context.optimize == .debug or context.optimize == .safe;

    return executable;
}

fn addEngineLibrary(
    context: BuildContextProps,
    third_party_libraries: ThirdPartyLibrariesProps,
) !*std.Build.Step.Compile {
    const b = context.builder;

    var engine_sources: std.ArrayList([]const u8) = .empty;
    for (engine_source_directories) |directory| {
        try engine_sources.appendSlice(b.allocator, try collectCppSources(b, directory));
    }
    try engine_sources.appendSlice(b.allocator, &engine_extra_sources);

    const engine_module = try createCppModule(context, engine_sources.items);
    linkThirdPartyLibraries(engine_module, third_party_libraries);

    return b.addLibrary(.{
        .name = "wheel_of_fear_engine",
        .linkage = .static,
        .root_module = engine_module,
    });
}

fn createCppModule(context: BuildContextProps, sources: []const []const u8) !*std.Build.Module {
    const b = context.builder;
    const module = b.createModule(.{
        .target = context.target,
        .optimize = context.optimize,
        .link_libc = true,
        .link_libcpp = true,
        .sanitize_c = .off,
    });

    module.addCSourceFiles(.{ .files = sources, .flags = &cpp_flags });
    module.addIncludePath(b.path("src"));
    module.addCMacro("WHEEL_OF_FEAR_ASSET_DIRECTORY", try quotedPathFromRoot(b, asset_directory));
    module.addCMacro("WHEEL_OF_FEAR_SHADER_DIRECTORY", try quotedPathFromRoot(b, shader_directory));

    return module;
}

fn quotedPathFromRoot(b: *std.Build, relative_path: []const u8) ![]const u8 {
    const native_path = try b.root.joinString(b.allocator, relative_path);
    const portable_path = try std.mem.replaceOwned(u8, b.allocator, native_path, "\\", "/");

    return b.fmt("\"{s}\"", .{portable_path});
}

fn collectCppSources(b: *std.Build, relative_directory: []const u8) ![]const []const u8 {
    const io = b.graph.io;
    var sources: std.ArrayList([]const u8) = .empty;

    var directory = try b.root.openDir(io, relative_directory, .{ .iterate = true });
    defer directory.close(io);

    var walker = try directory.walk(b.allocator);
    defer walker.deinit();

    while (try walker.next(io)) |entry| {
        if (entry.kind != .file or !std.mem.endsWith(u8, entry.basename, ".cpp")) continue;

        try sources.append(b.allocator, b.pathJoin(&.{ relative_directory, entry.path }));
    }

    std.mem.sort([]const u8, sources.items, {}, isPathLessThan);

    return sources.items;
}

fn isPathLessThan(_: void, left: []const u8, right: []const u8) bool {
    return std.mem.lessThan(u8, left, right);
}

fn resolveThirdPartyLibraries(context: BuildContextProps) !ThirdPartyLibrariesProps {
    const b = context.builder;
    const target_options = .{ .target = context.target, .optimize = context.optimize };

    return .{
        .sdl = b.dependency("sdl", target_options).artifact("SDL3"),
        .freetype = b.dependency("freetype", target_options).artifact("freetype"),
        .opus = try addOpusLibrary(context),
        .stb_include_directory = b.dependency("stb", .{}).path(""),
        .wgpu = b.lazyDependency(try wgpuDependencyName(context.target.result), .{}),
    };
}

fn linkThirdPartyLibraries(module: *std.Build.Module, libraries: ThirdPartyLibrariesProps) void {
    module.linkLibrary(libraries.sdl);
    module.linkLibrary(libraries.freetype);
    module.linkLibrary(libraries.opus);
    module.addIncludePath(libraries.stb_include_directory);

    const wgpu = libraries.wgpu orelse return;
    module.addIncludePath(wgpu.path("include"));
}

fn linkWgpuNative(
    context: BuildContextProps,
    module: *std.Build.Module,
    libraries: ThirdPartyLibrariesProps,
) void {
    const wgpu = libraries.wgpu orelse return;
    module.addObjectFile(wgpu.path("lib/libwgpu_native.a"));

    switch (context.target.result.os.tag) {
        .windows => for (windows_system_libraries) |library| module.linkSystemLibrary(library, .{}),
        .macos => for (macos_frameworks) |framework| module.linkFramework(framework, .{}),
        else => {},
    }
}

fn addOpusLibrary(context: BuildContextProps) !*std.Build.Step.Compile {
    const b = context.builder;
    const opus = b.dependency("opus", .{});

    const opus_module = b.createModule(.{
        .target = context.target,
        .optimize = context.optimize,
        .link_libc = true,
        .sanitize_c = .off,
    });
    opus_module.addCSourceFiles(.{
        .root = opus.path(""),
        .files = try readOpusSources(b, opus),
        .flags = &opus_c_flags,
    });
    for (opus_include_directories) |directory| opus_module.addIncludePath(opus.path(directory));

    const opus_library = b.addLibrary(.{
        .name = "opus",
        .linkage = .static,
        .root_module = opus_module,
    });
    opus_library.installHeadersDirectory(opus.path("include"), "", .{});

    return opus_library;
}

fn readOpusSources(b: *std.Build, opus: *std.Build.Dependency) ![]const []const u8 {
    const io = b.graph.io;
    var sources: std.ArrayList([]const u8) = .empty;

    for (opus_source_lists) |source_list| {
        const list_path = try opus.builder.root.join(b.allocator, source_list.file_name);
        const makefile_text = try list_path.root_dir.handle.readFileAlloc(io, list_path.sub_path, b.allocator, .unlimited);
        try appendMakefileVariableValues(b, &sources, makefile_text, source_list.variable_name);
    }

    return sources.items;
}

fn appendMakefileVariableValues(
    b: *std.Build,
    values: *std.ArrayList([]const u8),
    makefile_text: []const u8,
    variable_name: []const u8,
) !void {
    var lines = std.mem.splitScalar(u8, makefile_text, '\n');
    var is_inside_variable = false;

    while (lines.next()) |raw_line| {
        const line = std.mem.trim(u8, raw_line, " \t\r");

        if (!is_inside_variable) {
            const assignment = std.mem.indexOfScalar(u8, line, '=') orelse continue;
            const name = std.mem.trim(u8, line[0..assignment], " \t");
            is_inside_variable = std.mem.eql(u8, name, variable_name);
            continue;
        }

        const value = std.mem.trim(u8, std.mem.trimEnd(u8, line, "\\"), " \t");
        if (value.len > 0) try values.append(b.allocator, value);
        if (!std.mem.endsWith(u8, line, "\\")) return;
    }
}

fn wgpuDependencyName(target: std.Target) ![]const u8 {
    return switch (target.os.tag) {
        .linux => switch (target.cpu.arch) {
            .x86_64 => "wgpu_linux_x86_64",
            .aarch64 => "wgpu_linux_aarch64",
            else => error.UnsupportedWgpuTarget,
        },
        .windows => switch (target.cpu.arch) {
            .x86_64 => "wgpu_windows_x86_64_gnu",
            else => error.UnsupportedWgpuTarget,
        },
        .macos => switch (target.cpu.arch) {
            .aarch64 => "wgpu_macos_aarch64",
            .x86_64 => "wgpu_macos_x86_64",
            else => error.UnsupportedWgpuTarget,
        },
        else => error.UnsupportedWgpuTarget,
    };
}

fn installRuntimeData(b: *std.Build) void {
    b.installDirectory(.{
        .source_dir = b.path(asset_directory),
        .install_dir = .bin,
        .install_subdir = "assets",
    });
    b.installDirectory(.{
        .source_dir = b.path(shader_directory),
        .install_dir = .bin,
        .install_subdir = "shaders",
    });
}
