const std = @import("std");

const N = 1_000_000;

fn makeArr() ![]i32 {
    const alloc = std.heap.page_allocator;
    const arr = try alloc.alloc(i32, N);
    var prng = std.Random.DefaultPrng.init(42);
    const rand = prng.random();
    for (arr) |*x| {
        x.* = @intCast(rand.intRangeAtMost(u8, 0, 255));
    }
    return arr;
}

fn printHeader() !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});
}

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    const ns = total_ns / iters;
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, ns, iters });
}

fn benchModulo3(arr: []const i32) i32 {
    var sum: i32 = 0;
    for (arr) |x| {
        sum +%= @rem(x, 3);
    }
    return sum;
}

fn benchModulo128(arr: []const i32) i32 {
    var sum: i32 = 0;
    for (arr) |x| {
        sum +%= @rem(x, 128);
    }
    return sum;
}

pub fn main() !void {
    const arr = try makeArr();
    defer std.heap.page_allocator.free(arr);

    try printHeader();

    // Benchmark modulo 3
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            std.mem.doNotOptimizeAway(benchModulo3(arr));
            total += timer.read();
        }
        try printResult("BM_Modulo3", total, iters);
    }

    // Benchmark modulo 128
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            std.mem.doNotOptimizeAway(benchModulo128(arr));
            total += timer.read();
        }
        try printResult("BM_Modulo128", total, iters);
    }
}
