#include <debug.h>
#include <sifrpc.h>

int main(void)
{
    sceSifInitRpc(0);
    init_scr();
    scr_clear();
    scr_setXY(10, 10);
    scr_printf("Numbers Station\n");
    scr_printf("Native ARM64 Build Verified\n");

    for (;;) {
        __asm__ volatile("nop");
    }
}
