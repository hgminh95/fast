const std = @import("std");

const WithoutAlignment = struct {
    x: i32 = 0,
    y: i32 = 0,
    z: i32 = 0,
};

const WithAlignment = struct {
    x: i32 = 0,
    y: i32 = 0,
    z: i32 = 0,
    _padding: [52]u8 = .{0} ** 52, // pad to 64 bytes
};

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

fn benchFalseSharing(comptime T: type) !void {
    const alloc = std.heap.page_allocator;
    const arr = try alloc.alloc(T, 100_000);
    defer alloc.free(arr);
    @memset(arr, .{});

    var total: u64 = 0;
    var iters: u64 = 0;
    while (total < 2_000_000_000) : (iters += 1) {
        var timer = try std.time.Timer.start();
        const t1 = try std.Thread.spawn(.{}, struct {
            fn run(a: []T) void {
                var i: usize = 0;
                while (i < a.len) : (i += 2) {
                    a[i].x = @intCast(i);
                    a[i].y = @intCast(a.len - i);
                    a[i].z = @intCast(a.len + i);
                }
            }
        }.run, .{arr});
        const t2 = try std.Thread.spawn(.{}, struct {
            fn run(a: []T) void {
                var i: usize = 1;
                while (i < a.len) : (i += 2) {
                    a[i].x = @intCast(i);
                    a[i].y = @intCast(a.len - i);
                    a[i].z = @intCast(a.len + i);
                }
            }
        }.run, .{arr});
        t1.join();
        t2.join();
        total += timer.read();
    }
    const name = if (T == WithoutAlignment) "BM_FalseSharing<NoAlign>" else "BM_FalseSharing<Aligned>";
    try printResult(name, total, iters);
}

pub fn main() !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    try benchFalseSharing(WithoutAlignment);
    try benchFalseSharing(WithAlignment);
}
