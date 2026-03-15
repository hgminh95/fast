const std = @import("std");

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

pub fn main() !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    // True sharing: all threads contend on same atomic
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            var counter = std.atomic.Value(i32).init(0);
            var timer = try std.time.Timer.start();
            var threads: [4]std.Thread = undefined;
            for (&threads) |*t| {
                t.* = try std.Thread.spawn(.{}, struct {
                    fn run(c: *std.atomic.Value(i32)) void {
                        for (0..100_000) |_| {
                            _ = c.fetchAdd(1, .monotonic);
                        }
                    }
                }.run, .{&counter});
            }
            for (&threads) |t| t.join();
            std.mem.doNotOptimizeAway(counter.load(.monotonic));
            total += timer.read();
        }
        try printResult("BM_TrueSharing", total, iters);
    }

    // No true sharing: local accumulation
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 2_000_000_000) : (iters += 1) {
            var counter = std.atomic.Value(i32).init(0);
            var timer = try std.time.Timer.start();
            var threads: [4]std.Thread = undefined;
            for (&threads) |*t| {
                t.* = try std.Thread.spawn(.{}, struct {
                    fn run(c: *std.atomic.Value(i32)) void {
                        var local: i32 = 0;
                        for (0..100_000) |_| local += 1;
                        _ = c.fetchAdd(local, .monotonic);
                    }
                }.run, .{&counter});
            }
            for (&threads) |t| t.join();
            std.mem.doNotOptimizeAway(counter.load(.monotonic));
            total += timer.read();
        }
        try printResult("BM_NoTrueSharing", total, iters);
    }
}
