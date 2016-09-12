#include "share.h"
#include "static.h"
int test_add(int x, int y)
{
    // 调用static里面的方法
    return add(x, y);
}
