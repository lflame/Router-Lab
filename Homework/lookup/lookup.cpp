#include "router.h"
#include <stdint.h>
#include <stdlib.h>


const int MAXN = 105;
RoutingTableEntry table[MAXN];
bool enabled[MAXN];

/*
typedef struct {
  uint32_t addr; // 大端序，IPv4 地址
  uint32_t len; // 小端序，前缀长度
  uint32_t if_index; // 小端序，出端口编号
  uint32_t nexthop; // 大端序，下一跳的 IPv4 地址
  bool enabled; // Whether is enabled
} RoutingTableEntry;
   约定 addr 和 nexthop 以 **大端序** 存储。
   这意味着 1.2.3.4 对应 0x04030201 而不是 0x01020304。
   保证 addr 仅最低 len 位可能出现非零。
   当 nexthop 为零时这是一条直连路由。
   你可以在全局变量中把路由表以一定的数据结构格式保存下来。
 */

extern uint32_t convertBigSmallEndian32(uint32_t num);
extern uint32_t getMaskFromLen(uint32_t len);
extern uint32_t getNetworkSegment(uint32_t addr, uint32_t len);

int find(const RoutingTableEntry &entry) {
  // 找到 addr 和 len 同 entry 均相同的表项，返回其下标，未找到则返回 -1
  for (int i = 0; i < MAXN; ++i) {
    if (table[i].addr == entry.addr && table[i].len == entry.len && enabled[i])
      return i;
  }
  return -1;
}

int findEmpty() {
  // 找到一个空表项下标
  for (int i = 0; i < MAXN; ++i) {
    if (enabled[i] == false)
      return i;
  }
  return -1;
}

/**
 * @brief 插入/删除一条路由表表项
 * @param insert 如果要插入则为 true ，要删除则为 false
 * @param entry 要插入/删除的表项
 * 
 * 插入时如果已经存在一条 addr 和 len 都相同的表项，则替换掉原有的。
 * 删除时按照 addr 和 len 匹配。
 */

void update(bool insert, RoutingTableEntry entry) {
  // NOTE: 注意这里对 entry 进行了子网掩码的与操作，以保证entry的addr的主机标识为0，即仅最低 len 位可能出现非零
  entry.addr = getNetworkSegment(entry.addr, entry.len);
  int ind = find(entry);
  if (ind != -1) {
    if (insert) {
      enabled[ind] = true;
      table[ind] = entry;
    } else {
      enabled[ind] = false;
    }
  } else if (insert) {
    ind = findEmpty();
    table[ind] = entry;
    enabled[ind] = true;
  }
}

bool match(const RoutingTableEntry &entry, uint32_t addr) {
  // 判断地址 addr 是否处在 entry 所表示的网段中（由entry.addr和entry.len确定网段）
  uint32_t addr2 = convertBigSmallEndian32(entry.addr);
  addr = convertBigSmallEndian32(addr);
  for (int i = 0; i < entry.len; ++i) {
    if ((addr & (1 << (31-i))) != (addr2 & (1 << (31-i))))
      return false;
  }
  return true;
}

/**
 * @brief 进行一次路由表的查询，按照最长前缀匹配原则
 * @param addr 需要查询的目标地址，大端序
 * @param nexthop 如果查询到目标，把表项的 nexthop 写入
 * @param if_index 如果查询到目标，把表项的 if_index 写入
 * @return 查到则返回 true ，没查到则返回 false
 */
bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index) {
  int ind = -1;
  for (int i = 0; i < MAXN; ++i) {
    if (enabled[i] && match(table[i], addr)) {
      if (ind == -1 || table[i].len > table[ind].len)
        ind = i;
    }
  }
  if (ind == -1)
    return false;
  *nexthop = table[ind].nexthop;
  *if_index = table[ind].if_index;
  return true;
}
