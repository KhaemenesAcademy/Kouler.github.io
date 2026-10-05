#ifndef STOS_KOULER_H
#define STOS_KOULER_H
#include <stddef.h>
typedef enum kouler_status {
    KOULER_OK=0, KOULER_EINVAL=1, KOULER_EAUTHORITY=2,
    KOULER_EPREIMAGE=3, KOULER_EPOSTIMAGE=4,
    KOULER_EIO=5, KOULER_EOVERFLOW=6
} kouler_status;
typedef struct kouler_receipt {
    unsigned preimage_verified;
    unsigned replacement_committed;
    unsigned postimage_verified;
    size_t source_bytes;
    size_t output_bytes;
    size_t removed_bytes;
} kouler_receipt;
kouler_status kouler_main_directory_recovery(kouler_receipt *receipt);
#endif
