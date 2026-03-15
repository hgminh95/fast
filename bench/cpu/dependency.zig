const std = @import("std");

fn makeArr() ![]i32 {
    const alloc = std.heap.page_allocator;
    const arr = try alloc.alloc(i32, 90);
    var prng = std.Random.DefaultPrng.init(42);
    const rand = prng.random();
    for (arr) |*x| x.* = @intCast(rand.intRangeAtMost(u8, 0, 255));
    return arr;
}

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

pub fn main() !void {
    const a = try makeArr();
    defer std.heap.page_allocator.free(a);
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    // Dependency chain
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (0..90) |i| {
                sum *%= 10;
                sum +%= a[i];
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_Dependency", total, iters);
    }

    // Less dependency
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            var i: usize = 0;
            while (i < 90) : (i += 3) {
                sum *%= 1000;
                sum +%= a[i] *% 100;
                sum +%= a[i + 1] *% 10;
                sum +%= a[i + 2];
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_LessDependency", total, iters);
    }
}
