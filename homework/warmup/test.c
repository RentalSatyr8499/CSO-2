#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"

// Utility to free the result of string_split
static void free_words(char **words, int n) {
    for (int i = 0; i < n; i++) {
        free(words[i]);
    }
    free(words);
}

// Utility to check a single test case
static int check_case(
    const char *label,
    char **result, int size,
    const char *expected[], int expected_size
) {
    int ok = 1;

    if (size != expected_size) {
        printf("%s: FAIL (size %d != %d)\n", label, size, expected_size);
        return 0;
    }

    for (int i = 0; i < size; i++) {
        if (strcmp(result[i], expected[i]) != 0) {
            printf("%s: FAIL (word %d: got \"%s\", expected \"%s\")\n",
                   label, i, result[i], expected[i]);
            ok = 0;
        }
    }

    if (ok) {
        printf("%s: PASS\n", label);
    }

    return ok;
}

int main(void) {
    int all_ok = 1;

    // ------------------------------------------------------------
    // Test 1: "foo" with ":" → ["foo"]
    // ------------------------------------------------------------
    {
        int size = 12345;  // ANY_VALUE
        char **result = string_split("foo", ":", &size);

        const char *expected[] = { "foo" };
        all_ok &= check_case("Test 1", result, size, expected, 1);

        free_words(result, size);
    }

    // ------------------------------------------------------------
    // Test 2a: "foo:bar:quux" with ":" → ["foo","bar","quux"]
    // ------------------------------------------------------------
    {
        int size;
        char **result = string_split("foo:bar:quux", ":", &size);

        const char *expected[] = { "foo", "bar", "quux" };
        all_ok &= check_case("Test 2a", result, size, expected, 3);

        free_words(result, size);
    }

    // ------------------------------------------------------------
    // Test 2b: "foo:bar!quux" with "!:" → ["foo","bar","quux"]
    // ------------------------------------------------------------
    {
        int size;
        char **result = string_split("foo:bar!quux", "!:", &size);

        const char *expected[] = { "foo", "bar", "quux" };
        all_ok &= check_case("Test 2b", result, size, expected, 3);

        free_words(result, size);
    }

    // ------------------------------------------------------------
    // Test 2c: "foo:bar!quux" with ":!" → ["foo","bar","quux"]
    // ------------------------------------------------------------
    {
        int size;
        char **result = string_split("foo:bar!quux", ":!", &size);

        const char *expected[] = { "foo", "bar", "quux" };
        all_ok &= check_case("Test 2c", result, size, expected, 3);

        free_words(result, size);
    }

    // ------------------------------------------------------------
    // Test 3: ":foo!:bar::quux!" with ":!" → ["", "foo", "bar", "quux", ""]
    // ------------------------------------------------------------
    {
        int size;
        char **result = string_split(":foo!:bar::quux!", ":!", &size);

        const char *expected[] = { "", "foo", "bar", "quux", "" };
        all_ok &= check_case("Test 3", result, size, expected, 5);

        free_words(result, size);
    }

    // ------------------------------------------------------------
    // Final result
    // ------------------------------------------------------------
    if (all_ok) {
        printf("\nAll tests PASSED\n");
        return 0;
    } else {
        printf("\nSome tests FAILED\n");
        return 1;
    }
}
