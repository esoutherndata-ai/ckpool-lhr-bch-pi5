/*
 * CashAddr case-insensitivity test suite.
 *
 * Policy: the node (generator_checkaddr) is the sole validity gatekeeper.
 * The local decoder (address_to_txn) accepts any case — uppercase, lowercase,
 * and mixed-case CashAddr all decode to the same hash160 as their canonical
 * lowercase form.  This matches upstream BTC ckpool logic where address_to_txn
 * is a pure trusted converter operating on node-validated input.
 *
 * Prefix handling: prefixed (bchtest:qq...) and unprefixed (qq...) are treated
 * as distinct usernames by the pool — that is a user configuration choice.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "../test_common.h"
#include "libckpool.h"

static void assert_h160_eq(const char *txn_a, const char *txn_b,
                           const char *label_a, const char *label_b)
{
	if (memcmp(&txn_a[3], &txn_b[3], 20) != 0) {
		fprintf(stderr, "FAIL: hash160 mismatch between %s and %s\n",
		        label_a, label_b);
		exit(1);
	}
}

static void assert_txn_len(int len, int expected, const char *label)
{
	if (len != expected) {
		fprintf(stderr, "FAIL: %s decode returned %d, expected %d\n",
		        label, len, expected);
		exit(1);
	}
}

/* Uppercase and lowercase unprefixed CashAddr must decode to same hash160 */
static void test_unprefixed_case_insensitive(void)
{
	char txn_lower[25] = {0}, txn_upper[25] = {0}, txn_mixed[25] = {0};
	int len;

	const char *lower = "qpm2qsznhks23z7629mms6s4cwef74vcwvy22gdx6a";
	const char *upper = "QPM2QSZNHKS23Z7629MMS6S4CWEF74VCWVY22GDX6A";
	const char *mixed = "Qpm2QSznhks23z7629mms6s4cwef74vcwvY22GDX6a";

	len = address_to_txn(txn_lower, lower, false, false);
	assert_txn_len(len, 25, "unprefixed lowercase");

	len = address_to_txn(txn_upper, upper, false, false);
	assert_txn_len(len, 25, "unprefixed uppercase");

	len = address_to_txn(txn_mixed, mixed, false, false);
	assert_txn_len(len, 25, "unprefixed mixed-case");

	assert_h160_eq(txn_lower, txn_upper, "unprefixed lowercase", "unprefixed uppercase");
	assert_h160_eq(txn_lower, txn_mixed, "unprefixed lowercase", "unprefixed mixed-case");

	printf("  Unprefixed case-insensitive: PASS\n");
}

/* Uppercase and lowercase prefixed CashAddr must decode to same hash160 */
static void test_prefixed_case_insensitive(void)
{
	char txn_lower[25] = {0}, txn_upper[25] = {0}, txn_mixed[25] = {0};
	int len;

	const char *lower = "bitcoincash:qpm2qsznhks23z7629mms6s4cwef74vcwvy22gdx6a";
	const char *upper = "BITCOINCASH:QPM2QSZNHKS23Z7629MMS6S4CWEF74VCWVY22GDX6A";
	const char *mixed = "BitcoinCash:qpm2qsznhks23z7629mms6s4cwef74vcwvy22gdx6a";

	len = address_to_txn(txn_lower, lower, false, false);
	assert_txn_len(len, 25, "prefixed lowercase");

	len = address_to_txn(txn_upper, upper, false, false);
	assert_txn_len(len, 25, "prefixed uppercase");

	len = address_to_txn(txn_mixed, mixed, false, false);
	assert_txn_len(len, 25, "prefixed mixed-case");

	assert_h160_eq(txn_lower, txn_upper, "prefixed lowercase", "prefixed uppercase");
	assert_h160_eq(txn_lower, txn_mixed, "prefixed lowercase", "prefixed mixed-case");

	printf("  Prefixed case-insensitive: PASS\n");
}

/* bchtest: prefix variant */
static void test_bchtest_case_insensitive(void)
{
	char txn_lower[25] = {0}, txn_upper[25] = {0};
	int len;

	const char *lower = "bchtest:qq0ehslc4zkj70ljyyhhs74a85dlzkkvh5efglmhus";
	const char *upper = "BCHTEST:QQ0EHSLC4ZKJ70LJYYHHS74A85DLZKKVH5EFGLMHUS";

	len = address_to_txn(txn_lower, lower, false, false);
	assert_txn_len(len, 25, "bchtest lowercase");

	len = address_to_txn(txn_upper, upper, false, false);
	assert_txn_len(len, 25, "bchtest uppercase");

	assert_h160_eq(txn_lower, txn_upper, "bchtest lowercase", "bchtest uppercase");

	printf("  bchtest: case-insensitive: PASS\n");
}

/* bchreg: deterministic vector */
static void test_bchreg_case_insensitive(void)
{
	char txn_lower[25] = {0}, txn_upper[25] = {0};
	const unsigned char expected_h160[20] = {
		0x81, 0x9b, 0x2a, 0x65, 0x57, 0xb6, 0x03, 0xf7, 0x06, 0x46,
		0x79, 0xbf, 0x00, 0xc5, 0xae, 0x4d, 0xf2, 0xee, 0xfa, 0xf6
	};
	int len;

	const char *lower = "bchreg:qzqek2n927mq8acxgeum7qx94exl9mh67cd5hkczx3";
	const char *upper = "BCHREG:QZQEK2N927MQ8ACXGEUM7QX94EXL9MH67CD5HKCZX3";

	len = address_to_txn(txn_lower, lower, false, false);
	assert_txn_len(len, 25, "bchreg lowercase");
	if (memcmp(&txn_lower[3], expected_h160, 20) != 0) {
		fprintf(stderr, "FAIL: bchreg lowercase hash160 mismatch\n");
		exit(1);
	}

	len = address_to_txn(txn_upper, upper, false, false);
	assert_txn_len(len, 25, "bchreg uppercase");

	assert_h160_eq(txn_lower, txn_upper, "bchreg lowercase", "bchreg uppercase");

	printf("  bchreg: case-insensitive + deterministic vector: PASS\n");
}

/* Legacy Base58 is always passed through verbatim — case is fixed by definition */
static void test_legacy_unaffected(void)
{
	char txn[25] = {0};
	int len;

	len = address_to_txn(txn, "n2zYPuMkGhkeTZ5kn2PLbZDZQ3xpJixsAo", false, false);
	assert_txn_len(len, 25, "legacy P2PKH testnet");

	printf("  Legacy Base58 unaffected: PASS\n");
}

int main(void)
{
	printf("CashAddr case-insensitivity tests\n");
	run_test(test_unprefixed_case_insensitive);
	run_test(test_prefixed_case_insensitive);
	run_test(test_bchtest_case_insensitive);
	run_test(test_bchreg_case_insensitive);
	run_test(test_legacy_unaffected);
	printf("All CashAddr case-insensitivity tests passed!\n");
	return 0;
}

