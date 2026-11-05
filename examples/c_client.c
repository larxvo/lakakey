/* C FFI example for native adapters, not a platform IME. */
#include "lakakey_c.h"
#include <stdio.h>
int main(void) {
    lk_handle* handle=lk_create(0,0); // Telex, modern
    if(!handle) return 1;
    const char* keystrokes="tieengs";
    lk_result r={0};
    for(const unsigned char* p=(const unsigned char*)keystrokes;*p;p++)
        r=lk_feed(handle,(uint32_t)*p);
    r=lk_commit(handle);
    printf("%s\n",r.commit);
    lk_destroy(handle);
    return 0;
}
