const std = @import("std");

const N = 1_000_000;
const PAGE_STRIDE = 4096 / @sizeOf(i32);

fn makeArr() ![]i32 {
    const alloc = std.heap.page_allocator;
    const arr = try alloc.alloc(i32, N);
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
    const arr = try makeArr();
    defer std.heap.page_allocator.free(arr);
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (arr) |x| sum +%= x;
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_SequentialAccess", total, iters);
    }

    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            var idx: usize = 0;
            for (0..N) |_| {
                sum +%= arr[idx];
                idx = (idx + PAGE_STRIDE) % N;
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_PageStrideAccess", total, iters);
    }
}
