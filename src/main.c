#include "kouler.h"
#include <stdio.h>
int main(void){
    kouler_receipt r;
    kouler_status s=kouler_main_directory_recovery(&r);
    printf("KOULER_STATUS=%d\n",(int)s);
    printf("PREIMAGE_VERIFIED=%u\n",r.preimage_verified);
    printf("REPLACEMENT_COMMITTED=%u\n",r.replacement_committed);
    printf("POSTIMAGE_VERIFIED=%u\n",r.postimage_verified);
    printf("SOURCE_BYTES=%zu\n",r.source_bytes);
    printf("OUTPUT_BYTES=%zu\n",r.output_bytes);
    printf("REMOVED_BYTES=%zu\n",r.removed_bytes);
    return s==KOULER_OK?0:1;
}
