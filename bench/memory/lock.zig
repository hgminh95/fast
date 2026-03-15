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

    // Sum with local accumulation then atomic add
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            var sum = std.atomic.Value(i32).init(0);
            var timer = try std.time.Timer.start();
            var threads: [4]std.Thread = undefined;
            for (&threads, 0..) |*t, idx| {
                t.* = try std.Thread.spawn(.{}, struct {
                    fn run(a: []const i32, s: *std.atomic.Value(i32), id: usize) void {
                        var partial: i32 = 0;
                        const start = id * a.len / 4;
                        const end = (id + 1) * a.len / 4;
                        for (a[start..end]) |x| partial +%= x;
                        _ = s.fetchAdd(partial, .monotonic);
                    }
                }.run, .{ arr, &sum, idx });
            }
            for (&threads) |t| t.join();
            std.mem.doNotOptimizeAway(sum.load(.monotonic));
            total += timer.read();
        }
        try printResult("BM_SumWithLocalAtomic", total, iters);
    }

    // Sum with naive atomic (every iteration)
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            var sum = std.atomic.Value(i32).init(0);
            var timer = try std.time.Timer.start();
            var threads: [4]std.Thread = undefined;
            for (&threads, 0..) |*t, idx| {
                t.* = try std.Thread.spawn(.{}, struct {
                    fn run(a: []const i32, s: *std.atomic.Value(i32), id: usize) void {
                        const start = id * a.len / 4;
                        const end = (id + 1) * a.len / 4;
                        for (a[start..end]) |x| _ = s.fetchAdd(x, .monotonic);
                    }
                }.run, .{ arr, &sum, idx });
            }
            for (&threads) |t| t.join();
            std.mem.doNotOptimizeAway(sum.load(.monotonic));
            total += timer.read();
        }
        try printResult("BM_SumWithNaiveAtomic", total, iters);
    }
}
