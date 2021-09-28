// SPDX-License-Identifier: zlib-acknowledgement

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "static-elf.h"

void
test(void **state)
{
  ElfLoadResult elf_load_result = elf_load("../tests/sample-lib.so");

  int (*add_int)(int, int) = elf_lookup_function(&elf_load_result, "add_int");
  assert_int_equal(add_int(10, 20), 30);

  int (*sub_int)(int, int) = elf_lookup_function(&elf_load_result, "sub_int");
  assert_int_equal(sub_int(20, 10), 10);
}

int
main(void)
{
  const struct CMUnitTest tests[] =
  {
    cmocka_unit_test(test),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
