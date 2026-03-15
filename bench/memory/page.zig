const std = @import("std");

const N = 100_000_000;

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    // With page fault (no pre-touch)
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            const arr = try alloc.alloc(i32, N);
            var timer = try std.time.Timer.start();
            for (arr, 0..) |*x, i| x.* = @intCast(i);
            total += timer.read();
            std.mem.doNotOptimizeAway(arr.ptr);
            alloc.free(arr);
        }
        try printResult("BM_MinorPageFault", total, iters);
    }

    // Without page fault (pre-touch with fill)
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            const arr = try alloc.alloc(i32, N);
            @memset(arr, 0);
            var timer = try std.time.Timer.start();
            for (arr, 0..) |*x, i| x.* = @intCast(i);
            total += timer.read();
            std.mem.doNotOptimizeAway(arr.ptr);
            alloc.free(arr);
        }
        try printResult("BM_NoMinorPageFault", total, iters);
    }
}
