const std = @import("std");

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

fn nops(comptime count: usize) void {
    comptime var i = 0;
    inline while (i < count) : (i += 1) {
        asm volatile ("nop");
    }
}

fn benchJump(comptime nop_count: usize) void {
    for (0..100_000) |i| {
        nops(nop_count);
        std.mem.doNotOptimizeAway(i);
    }
}

pub fn main() !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    inline for (.{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32 }) |n| {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 500_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            benchJump(n);
            total += timer.read();
        }
        try printResult("BM_Jump<" ++ std.fmt.comptimePrint("{}", .{n}) ++ ">", total, iters);
    }
}
