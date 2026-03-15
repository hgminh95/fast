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
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    const unsorted = try makeArr();
    defer std.heap.page_allocator.free(unsorted);

    const sorted = try std.heap.page_allocator.alloc(i32, N);
    defer std.heap.page_allocator.free(sorted);
    @memcpy(sorted, unsorted);
    std.mem.sort(i32, sorted, {}, std.sort.asc(i32));

    // Unsorted with branch
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (unsorted) |x| {
                if (x > 128) sum +%= x;
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_UnsortedArray", total, iters);
    }

    // Unsorted branchless
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (unsorted) |x| {
                sum +%= x * @as(i32, @intFromBool(x > 128));
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_UnsortedArrayNoBranch", total, iters);
    }

    // Sorted with branch
    {
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (sorted) |x| {
                if (x > 128) sum +%= x;
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_SortedArray", total, iters);
    }
}
