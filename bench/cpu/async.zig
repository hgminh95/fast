const std = @import("std");

const N = 100_000;

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

fn makeChain(alloc: std.mem.Allocator, seed: u64) ![]i32 {
    const arr = try alloc.alloc(i32, N);
    var indices: [N]usize = undefined;
    for (&indices, 0..) |*x, i| x.* = i;
    var prng = std.Random.DefaultPrng.init(seed);
    prng.random().shuffle(usize, &indices);
    for (0..N - 1) |i| arr[indices[i]] = @intCast(indices[i + 1]);
    arr[indices[N - 1]] = @intCast(indices[0]);
    return arr;
}

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    const arr1 = try makeChain(alloc, 42);
    defer alloc.free(arr1);
    const arr2 = try makeChain(alloc, 123);
    defer alloc.free(arr2);

    // One chain
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            var p: usize = 0;
            for (0..N) |_| {
                p = @intCast(@as(u32, @bitCast(arr1[p])));
                sum +%= @intCast(p);
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_OneChain", total, iters);
    }

    // Two chains
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            var p1: usize = 0;
            var p2: usize = 0;
            for (0..N) |_| {
                p1 = @intCast(@as(u32, @bitCast(arr1[p1])));
                p2 = @intCast(@as(u32, @bitCast(arr2[p2])));
                sum +%= @as(i32, @intCast(p1)) +% @as(i32, @intCast(p2));
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_TwoChains", total, iters);
    }
}
