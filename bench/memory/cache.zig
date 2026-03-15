const std = @import("std");

fn makeArr(n: usize) ![]i32 {
    const alloc = std.heap.page_allocator;
    const arr = try alloc.alloc(i32, n);
    var prng = std.Random.DefaultPrng.init(42);
    const rand = prng.random();
    for (arr) |*x| x.* = @intCast(rand.intRangeAtMost(u8, 0, 255));
    return arr;
}

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

fn benchCache(comptime SIZE: usize, arr1: []const i32, arr2: []const i32) i32 {
    var sum: i32 = 0;
    var j: usize = 0;
    while (j < 1024 / SIZE) : (j += 1) {
        var i: usize = 0;
        while (i < SIZE) : (i += 1) {
            sum +%= arr1[i] +% arr2[i];
        }
    }
    return sum;
}

fn benchStride(arr: []i32, stride: usize) i32 {
    for (arr, 0..) |*x, i| x.* = @intCast((i + stride) % arr.len);
    var sum: i32 = 0;
    var p: usize = 0;
    for (0..arr.len) |_| {
        sum +%= arr[p];
        p = @intCast(@as(u32, @bitCast(arr[p])));
    }
    return sum;
}

pub fn main() !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    inline for (.{ 1, 2, 4, 8, 16, 64, 128, 256 }) |size| {
        const arr1 = try makeArr(size);
        const arr2 = try makeArr(size);
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 500_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            std.mem.doNotOptimizeAway(benchCache(size, arr1, arr2));
            total += timer.read();
        }
        try printResult("BM_Cache<" ++ std.fmt.comptimePrint("{}", .{size}) ++ ">", total, iters);
        std.heap.page_allocator.free(arr1);
        std.heap.page_allocator.free(arr2);
    }

    const arr = try makeArr(1_000_000);
    defer std.heap.page_allocator.free(arr);

    inline for (.{ 1, 8, 64, 512, 4096, 32768 }) |stride| {
        {
            var total: u64 = 0;
            var iters: u64 = 0;
            while (total < 1_000_000_000) : (iters += 1) {
                var timer = try std.time.Timer.start();
                std.mem.doNotOptimizeAway(benchStride(@constCast(arr), stride));
                total += timer.read();
            }
            try printResult("BM_StrideAccess/" ++ std.fmt.comptimePrint("{}", .{stride}), total, iters);
        }
    }
}
