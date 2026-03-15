const std = @import("std");

const N = 100_000;

fn printResult(name: []const u8, total_ns: u64, iters: u64) !void {
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {d:>8} ns {d:>12}\n", .{ name, total_ns / iters, iters });
}

const Node = struct {
    value: i32,
    next: ?*Node,
};

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    const w = std.io.getStdOut().writer();
    try w.print("{s:<40} {s:>12} {s:>12}\n", .{ "Benchmark", "Time", "Iterations" });
    try w.print("{s}\n", .{"-" ** 66});

    // Linked list (random pointer chasing)
    {
        const nodes = try alloc.alloc(Node, N);
        defer alloc.free(nodes);

        var indices: [N]usize = undefined;
        for (&indices, 0..) |*x, i| x.* = i;
        var prng = std.Random.DefaultPrng.init(42);
        prng.random().shuffle(usize, &indices);

        for (0..N - 1) |i| {
            nodes[indices[i]] = .{ .value = @intCast(indices[i]), .next = &nodes[indices[i + 1]] };
        }
        nodes[indices[N - 1]] = .{ .value = @intCast(indices[N - 1]), .next = null };

        const head: *Node = &nodes[indices[0]];

        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            var p: ?*Node = head;
            while (p) |node| {
                sum +%= node.value;
                p = node.next;
            }
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_LinkedList", total, iters);
    }

    // Array traversal
    {
        const arr = try alloc.alloc(i32, N);
        defer alloc.free(arr);
        for (arr, 0..) |*x, i| x.* = @intCast(i);

        var total: u64 = 0;
        var iters: u64 = 0;
        while (total < 1_000_000_000) : (iters += 1) {
            var timer = try std.time.Timer.start();
            var sum: i32 = 0;
            for (arr) |x| sum +%= x;
            std.mem.doNotOptimizeAway(sum);
            total += timer.read();
        }
        try printResult("BM_ArrayTraversal", total, iters);
    }
}
