### Links
1. https://github.com/jamesroutley/algorithms-and-data-structures/tree/master/hash-table/src
2. https://www.algolist.net/Data_structures/Hash_table/Open_addressing


### 如果在 insert 的时候不复用被标记为删除的元素的话，那不是会有很多被标记为删除的元素吗
是的，如果在插入时不复用那些标记为删除的元素（`HT_DELETED_ITEM`），会导致哈希表中出现大量的删除标记，随着时间推移，表中的空间将越来越多被这些无效的删除标记占据。这会导致以下两个问题：

1. **空间浪费**：这些删除标记占据了可用空间，使哈希表越来越满，实际可用的空间减少，降低存储效率。
2. **探查效率下降**：每次插入或查找操作都要遍历这些被标记为删除的位置，导致探查的路径变长，影响性能。

因此，**复用被标记为删除的元素位置** 是必要的，否则会导致性能和空间效率的下降。

### 如何正确复用删除标记的位置

在开放地址法（如线性探查）中，我们通过 `HT_DELETED_ITEM` 标记删除的元素，以保持冲突链的完整性。当在探查链上遇到 `HT_DELETED_ITEM` 时，插入操作可以安全地复用这个位置，因为它代表一个曾经使用过但现在空闲的空间。

### 改进的插入逻辑：复用删除标记位置
我们可以在插入时，**优先复用被标记为删除的元素位置**。具体步骤如下：

1. 当探查时遇到 `HT_DELETED_ITEM`，不要立即插入，而是继续探查链，检查是否有相同的键（避免重复插入）。
2. 如果整个探查链中没有找到相同的键，**将元素插入到最早遇到的删除标记位置**。

这样可以确保：
- 如果链中有相同的键，直接更新该键的值，避免重复插入。
- 如果没有相同的键，复用最早的删除标记位置，而不是一直探查直到找到空位置。

下面是插入函数的改进版：

```c
void ht_insert(ht_hash_table *ht, const char *key, const char *value) {
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    ht_item *item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item *cur_item = ht->items[index];
    int i = 1;
    int deleted_index = -1;  // 记录第一个被删除的位置

    // 查找空闲位置、删除标记，或者已经存在相同的key
    while (cur_item != NULL) {
        if (cur_item == &HT_DELETED_ITEM) {
            // 记录第一个找到的删除标记的位置
            if (deleted_index == -1) {
                deleted_index = index;
            }
        } else if (strcmp(cur_item->key, key) == 0) {
            // 如果找到相同key，更新值并退出
            ht_del_item(cur_item);
            ht->items[index] = item;
            return;
        }
        
        // 继续探查下一个位置
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }

    // 如果找到了删除标记的位置，就插入到那里
    if (deleted_index != -1) {
        ht->items[deleted_index] = item;
    } else {
        // 否则插入到第一个空闲位置
        ht->items[index] = item;
    }
    ht->count++;
}
```

### 核心改进：
1. **记录第一个删除标记的位置**：我们在探查时，如果遇到删除标记（`HT_DELETED_ITEM`），不会立即插入，而是继续查找是否有相同的键。如果没有相同的键，则优先复用第一个遇到的删除标记位置。
2. **复用删除标记位置**：如果找到删除标记位置并且整个探查链中没有相同的键，则复用该删除标记位置进行插入。

### 为什么这样有效？
- **复用删除标记**：复用了标记为删除的位置，避免哈希表中充满无效的删除标记，保持表的空间利用率。
- **保持冲突链完整性**：即使遇到删除标记，探查不会立即停止，依然会继续检查冲突链中的其他元素，从而确保查找和插入操作不会受到影响。
- **插入新元素**：在未找到相同键的情况下，会优先插入到删除标记位置，保证空间的合理利用。

### 总结
- **复用 `HT_DELETED_ITEM` 是必要的**，否则会浪费空间并导致探查效率下降。
- 改进后的插入逻辑可以确保删除标记位置被合理复用，同时不会打乱冲突链的完整性。
- 通过这种方法，你的哈希表在处理删除和插入时，既能保持高效的空间利用，又能确保探查效率不受影响。