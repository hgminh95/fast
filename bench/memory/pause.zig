const std = @import("std");
const builtin = @import("builtin");

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

fn spinPause() void {
    if (builtin.cpu.arch == .x86_64) {
        asm volatile ("pause");
    } else if (builtin.cpu.arch == .aarch64) {
        asm volatile ("yield");
    }
}

pub fn main() !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    var lock = std.atomic.Value(bool).init(false);

    // With pause
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            for (0..100_000) |_| {
                while (lock.swap(true, .acquire)) spinPause();
                lock.store(false, .release);
            }
            total += timer.read();
        }
        try printResult("BM_SpinlockWithPause", total, iters);
    }

    // Without pause
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            for (0..100_000) |_| {
                while (lock.swap(true, .acquire)) {}
                lock.store(false, .release);
            }
            total += timer.read();
        }
        try printResult("BM_SpinlockWithoutPause", total, iters);
    }
}
