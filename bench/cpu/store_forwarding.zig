const std = @import("std");

const N = 1_000_000;

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

const Elem = extern struct {
    part1: i32 = 0,
    part2: i32 = 0,
};

inline fn compilerFence() void {
    asm volatile ("" ::: "memory");
}

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    const arr = try alloc.alloc(Elem, N);
    defer alloc.free(arr);
    @memset(arr, .{});

    const x: i32 = 1;

    // Partial store then wide load — may fail store forwarding
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i64 = 0;
            for (arr) |*elem| {
                elem.part1 = 0;
                elem.part2 = x;
                compilerFence();
                sum +%= @as(*align(1) const i64, @ptrCast(elem)).*;
                std.mem.doNotOptimizeAway(sum);
            }
            total += timer.read();
        }
        try printResult("BM_StoreForwarding<Partial>", total, iters);
    }

    // Single wide store then wide load — store forwarding succeeds
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i64 = 0;
            for (arr) |*elem| {
                @as(*align(1) i64, @ptrCast(elem)).* = x;
                compilerFence();
                sum +%= @as(*align(1) const i64, @ptrCast(elem)).*;
                std.mem.doNotOptimizeAway(sum);
            }
            total += timer.read();
        }
        try printResult("BM_StoreForwarding<Full>", total, iters);
    }
}
