const std = @import("std");

const N = 1_000_000;

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

    // Single accumulator
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (arr) |x| sum +%= @as(i32, @intCast(@popCount(@as(u32, @bitCast(x)))));
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_FalseDependency", total, iters);
    }

    // Four accumulators
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var r0: i32 = 0;
            var r1: i32 = 0;
            var r2: i32 = 0;
            var r3: i32 = 0;
            var i: usize = 0;
            while (i < arr.len) : (i += 4) {
                r0 +%= @as(i32, @intCast(@popCount(@as(u32, @bitCast(arr[i])))));
                r1 +%= @as(i32, @intCast(@popCount(@as(u32, @bitCast(arr[i + 1])))));
                r2 +%= @as(i32, @intCast(@popCount(@as(u32, @bitCast(arr[i + 2])))));
                r3 +%= @as(i32, @intCast(@popCount(@as(u32, @bitCast(arr[i + 3])))));
            }
            std.mem.doNotOptimizeAway(r0 +% r1 +% r2 +% r3);
            total += timer.read();
        }
        try printResult("BM_NoFalseDependency", total, iters);
    }
}
