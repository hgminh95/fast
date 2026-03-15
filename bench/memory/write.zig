const std = @import("std");

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

const CacheLine = extern struct {
    data: [64]u8 align(64) = .{0} ** 64,
};

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    const N = 1_000_000;
    const arr = try alloc.alloc(CacheLine, N);
    defer alloc.free(arr);

    // Partial cache line write
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            for (arr, 0..) |*cl, i| cl.data[0] = @truncate(i);
            std.mem.doNotOptimizeAway(arr.ptr);
            total += timer.read();
        }
        try printResult("BM_PartialCacheLineWrite", total, iters);
    }

    // Full cache line write
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            for (arr, 0..) |*cl, i| @memset(&cl.data, @truncate(i));
            std.mem.doNotOptimizeAway(arr.ptr);
            total += timer.read();
        }
        try printResult("BM_FullCacheLineWrite", total, iters);
    }
}
