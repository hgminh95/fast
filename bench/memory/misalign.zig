const std = @import("std");

const N = 1_000_000;

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    // Aligned access
    {
        const arr = try alloc.alloc(i64, N);
        defer alloc.free(arr);
        @memset(arr, 42);

        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i64 = 0;
            for (arr) |x| sum +%= x;
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_AlignedAccess", total, iters);
    }

    // Misaligned access
    {
        const buf = try alloc.alloc(u8, N * @sizeOf(i64) + 3);
        defer alloc.free(buf);
        const ptr: [*]align(1) i64 = @ptrCast(buf.ptr + 3);
        for (0..N) |i| ptr[i] = 42;

        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i64 = 0;
            for (0..N) |i| sum +%= ptr[i];
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_MisalignedAccess", total, iters);
    }
}
