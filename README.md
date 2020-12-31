# SHA-256
Public domain C SHA-256 implementation.

# Example

```c
#include "sha256.h"

#include <stdio.h>
#include <string.h>

int main(void){
    /* Input text. */
    const char *text = "Hello, World!";

    /* Char array to store the hexadecimal SHA-256 string. */
    /* Must be 65 characters big (or larger). */
    /* The last character will be the null-character. */
    char hex[SHA256_HEX_SIZE];

    /* Compute SHA-256 sum. */
    sha256_hex(text, strlen(text), hex);

    /* Print result. */
    printf("The SHA-256 sum of \"%s\" is:\n\n", text);
    printf("%s\n\n", hex);

    return 0;
}
```

Output:

```
The SHA-256 sum of "Hello, World!" is:

dffd6021bb2bd5b0af676290809ec3a53191dd81c7f70a4b28688a362182986f

```

Compile and run with:

```bash
make example
./example
```

See [tests.c](https://github.com/983/SHA-256/blob/main/tests.c) for more usage examples:

- Compute streaming SHA-256 checksum of data that does not fit into memory.
- Compute SHA-256 checksum of a file.
- Verify a block of the Bitcoin blockchain.

# Tests

Run the tests with:

```bash
make tests
./tests
```
