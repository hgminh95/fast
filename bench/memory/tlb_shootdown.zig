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

    // Single thread
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (0..10_000_000) |i| sum +%= arr[i % N];
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_SingleThread", total, iters);
    }

    // With allocator thread causing TLB shootdowns
    {
        var running = std.atomic.Value(bool).init(true);
        const allocator_thread = try std.Thread.spawn(.{}, struct {
            fn run(r: *std.atomic.Value(bool)) void {
                const alloc = std.heap.page_allocator;
                while (r.load(.monotonic)) {
                    const mem = alloc.alloc(i32, 10000) catch continue;
                    alloc.free(mem);
                }
            }
        }.run, .{&running});

        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (0..1_000_000) |i| sum +%= arr[i % N];
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        running.store(false, .monotonic);
        allocator_thread.join();
        try printResult("BM_MultiThread", total, iters);
    }
}
