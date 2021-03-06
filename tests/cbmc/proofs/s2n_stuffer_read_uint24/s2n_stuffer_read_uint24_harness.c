/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <assert.h>
#include <cbmc_proof/cbmc_utils.h>
#include <cbmc_proof/make_common_datastructures.h>
#include <cbmc_proof/proof_allocators.h>

#include "api/s2n.h"
#include "stuffer/s2n_stuffer.h"

void s2n_stuffer_read_uint24_harness()
{
    struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();
    __CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));

    struct s2n_stuffer old_stuffer = *stuffer;
    uint32_t *         dest        = can_fail_malloc(sizeof(uint32_t *));

    /* Store a byte from the stuffer to compare after the read */
    struct store_byte_from_buffer old_byte_from_stuffer;
    save_byte_from_blob(&stuffer->blob, &old_byte_from_stuffer);

    if (s2n_stuffer_read_uint24(stuffer, dest) == S2N_SUCCESS) {
        assert(stuffer->read_cursor == old_stuffer.read_cursor + SIZEOF_UINT24);
        /* If successful, ensure uint was assembled correctly from stuffer */
        assert((( uint32_t )stuffer->blob.data[ old_stuffer.read_cursor ]) << 16
               | (( uint32_t )stuffer->blob.data[ old_stuffer.read_cursor + 1 ]) << 8
               | (( uint32_t )stuffer->blob.data[ old_stuffer.read_cursor + 2 ]) == *dest | *dest < (1 << (24 - 1)));
    } else {
        assert(stuffer->read_cursor == old_stuffer.read_cursor);
    }
    assert_stuffer_immutable_fields_after_read(stuffer, &old_stuffer, &old_byte_from_stuffer);
    assert(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));
}
