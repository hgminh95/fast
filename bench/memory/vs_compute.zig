const std = @import("std");

const N = 1_000_000;

const FooWithCache = struct {
    values: [30]i32 = .{0} ** 30,
    cached_bar: i32 = 0,

    fn bar(self: *FooWithCache) i32 {
        if (self.cached_bar == 0) {
            for (self.values, 0..) |v, i| {
                self.cached_bar +%= v *% @as(i32, @intCast(i));
            }
        }
        return self.cached_bar;
    }
};

const FooWithoutCache = struct {
    values: [30]i32 = .{0} ** 30,

    fn bar(self: *const FooWithoutCache) i32 {
        var res: i32 = 0;
        for (self.values, 0..) |v, i| {
            res +%= v *% @as(i32, @intCast(i));
        }
        return res;
    }
};

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    {
        const arr = try alloc.alloc(FooWithCache, N);
        defer alloc.free(arr);
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (arr) |*foo| sum +%= foo.bar();
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_FooWithCachedBar", total, iters);
    }

    {
        const arr = try alloc.alloc(FooWithoutCache, N);
        defer alloc.free(arr);
        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (arr) |*foo| sum +%= foo.bar();
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_FooWithoutCachedBar", total, iters);
    }
}
