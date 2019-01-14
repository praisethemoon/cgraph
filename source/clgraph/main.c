#include <cf4ocl2.h>
int main() {
    /* Variables. */
    CCLContext * ctx = NULL;
    /* Code. */
    ctx = ccl_context_new_from_menu(NULL);
    if (ctx == NULL) exit(-1);
    /* Destroy context wrapper. */
    ccl_context_destroy(ctx);
    return 0;
}
