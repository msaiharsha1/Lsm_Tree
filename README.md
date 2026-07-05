# LSM Tree (C++)

A simple LSM (Log-Structured Merge) tree implemented in C++ to learn how the basic idea works.

## How it's organized

- **Root** — the entry point for all writes. Stores recent data as a linked list, newest first.
- **Level** — a hash table of buckets sitting below the Root. Each bucket is a `LevelNode`.
- **LevelNode** — holds a small list of key-value pairs inside one bucket.
- **LSM** — the class you actually talk to; it wraps a Root and wires the levels together.

## How data moves

1. **Insert** — every new key-value pair goes straight into the Root's linked list.
2. **Flush** — once the Root has 4 entries, it's full. Everything in it gets pushed down into Level 1, and the Root empties out.
3. **Lookup** — check the Root's linked list first (since it has the newest writes), then fall through to Level 1 if the key isn't there.
4. **Delete** — no data is actually removed. Instead, the key is inserted again with a tombstone value (`-1`), so it looks "deleted" on future lookups.

## Example

```cpp
LSM db;

db.myInsert(1, 100);
db.myInsert(2, 200);
db.myInsert(3, 300);
db.myInsert(4, 400);   // Root is full, flushes to Level 1

cout << db.myLookup(2) << endl;   // 200

db.myDelete(2);
cout << db.myLookup(2) << endl;   // -1 (deleted)
```

## Honest limitations

- There's only ever one level below the Root — Level 1 never overflows further, even though the code has a `nextLevel` pointer meant for that.
- A tombstone and "key doesn't exist" return the same value (`-1`), so you can't tell them apart right now.
- No cleanup or compaction happens — Level 1 just keeps growing.
