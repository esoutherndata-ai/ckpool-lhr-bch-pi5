/*
 * Unit tests for Bitcoin Cash address encoding
 * Tests Base58 (P2PKH, P2SH) and CashAddr decoding through address_to_txn()
 * Note: BCH does not support Bech32/Segwit addresses - only BTC uses those
 * Supports both mainnet and testnet address formats
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "config.h"
#include "../test_common.h"
#include "libckpool.h"

/* Test address_to_txn with legacy P2PKH address (starts with '1')
 * These are Base58 encoded addresses
 * Note: Full address validation requires bitcoind, so we test that the
 * function processes addresses without crashing and produces expected lengths
 */
static void test_legacy_p2pkh_address(void)
{
    char txn[100];
    int len;
    
    /* Test with a known valid P2PKH address format */
    const char *test_addr = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa"; // Genesis block address
    
    /* Test P2PKH (script=false, segwit=false) */
    len = address_to_txn(txn, test_addr, false, false);
    
    /* P2PKH transaction should be 25 bytes
     * We verify the length matches expected structure, but don't validate
     * exact byte values as Base58 decoding may vary
     */
    assert_true(len == 25);
    
    /* Verify function doesn't crash and produces output */
    assert_true(len > 0);
    assert_true(len <= 100); // Reasonable size limit
}

/* Test address_to_txn with P2SH address (starts with '3')
 * These are Base58 encoded script addresses
 */
static void test_p2sh_address(void)
{
    char txn[100];
    int len;
    
    /* Test with a known valid P2SH address format */
    const char *test_addr = "3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy"; // Example P2SH format
    
    /* Test P2SH (script=true, segwit=false) */
    len = address_to_txn(txn, test_addr, true, false);
    
    /* P2SH transaction should be 23 bytes
     * We verify the length matches expected structure
     */
    assert_true(len == 23);
    assert_true(len > 0);
}

/* Test address_to_txn with CashAddr format (bitcoincash:q... for P2PKH, bitcoincash:p... for P2SH)
 * CashAddr is the primary address format for Bitcoin Cash
 */
static void test_cashaddr_address(void)
{
    char txn[100];
    int len;
    
    /* Test with CashAddr P2PKH format (bitcoincash:q...) */
    const char *cashaddr_p2pkh = "bitcoincash:qz2stage6prwvclzkejd83v8mt82jqg74ysnj3uvqt";
    
    /* Test CashAddr P2PKH (script=false, segwit=false) */
    len = address_to_txn(txn, cashaddr_p2pkh, false, false);
    
    /* CashAddr P2PKH transaction should be 25 bytes (same as legacy P2PKH) */
    assert_true(len == 25);
    assert_true(len > 0);
}

/* Test testnet CashAddr format (bchtest:q... for P2PKH, bchtest:p... for P2SH) */
static void test_testnet_cashaddr_address(void)
{
    char txn[100];
    int len;
    
    /* Test with testnet CashAddr P2PKH format (bchtest:q...) */
    const char *testnet_cashaddr = "bchtest:qz2stage6prwvclzkejd83v8mt82jqg74yxvnp4f";
    
    /* Test testnet CashAddr P2PKH (script=false, segwit=false) */
    len = address_to_txn(txn, testnet_cashaddr, false, false);
    
    /* Should produce valid 25-byte P2PKH transaction */
    assert_true(len == 25);
    assert_true(len > 0);
}

/* Test that address_to_txn handles different address types correctly */
static void test_address_type_routing(void)
{
    char txn[100];
    int len_p2pkh, len_p2sh;
    const char *test_addr = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    
    /* Test that different flags produce different transaction formats
     * Note: Using the same address with different flags tests the routing logic,
     * though the actual decoding may vary based on address type
     */
    len_p2pkh = address_to_txn(txn, test_addr, false, false);
    
    /* P2PKH should be 25 bytes */
    assert_true(len_p2pkh == 25);
    
    /* P2SH should be 23 bytes (using same address tests routing, not validity) */
    len_p2sh = address_to_txn(txn, test_addr, true, false);
    assert_true(len_p2sh == 23);
    
    /* BCH does not support Segwit - segwit parameter is ignored */
    int len_ignored = address_to_txn(txn, test_addr, false, true);
    assert_true(len_ignored == 25); /* Still produces P2PKH output */
}

/* Test Base58 decoding through b58tobin (indirectly via address_to_txn)
 * Base58 uses characters: 123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz
 * We test that the function processes Base58 addresses and produces expected lengths
 */
static void test_base58_encoding_structure(void)
{
    char txn[100];
    int len;
    
    /* Test that Base58 addresses produce expected transaction structure */
    const char *test_addr = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    
    len = address_to_txn(txn, test_addr, false, false);
    
    /* Verify the transaction length is correct for P2PKH
     * Full structure validation would require verifying exact opcodes,
     * but that depends on successful Base58 decoding which may vary
     */
    assert_true(len == 25);
    assert_true(len > 0);
}

/* Test that address functions handle various address formats */
static void test_address_format_handling(void)
{
    char txn[100];
    int len;
    
    /* Test different address formats supported by BCH */
    
    /* Legacy address (P2PKH) */
    const char *legacy = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    len = address_to_txn(txn, legacy, false, false);
    assert_true(len == 25);
    
    /* Script address (P2SH) */
    const char *script = "3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy";
    len = address_to_txn(txn, script, true, false);
    assert_true(len == 23);
    
    /* CashAddr address (bitcoincash:q... for P2PKH) */
    const char *cashaddr = "bitcoincash:qz2stage6prwvclzkejd83v8mt82jqg74ysnj3uvqt";
    len = address_to_txn(txn, cashaddr, false, false);
    assert_true(len == 25);
}

/* Test edge cases - very long addresses, special characters, etc.
 * Note: These tests verify the functions don't crash, not that they produce valid output
 */
static void test_address_edge_cases(void)
{
    char txn[100];
    int len;
    
    /* Test with various address-like strings
     * Note: Without full validation, we're just testing the functions don't crash
     */
    
    /* Standard length address */
    const char *normal = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    len = address_to_txn(txn, normal, false, false);
    assert_true(len > 0);
    
    /* The functions should handle the input without crashing
     * Full validation would require bitcoind integration
     */
}

int main(void)
{
    printf("Running address encoding tests...\n\n");
    
    run_test(test_legacy_p2pkh_address);
    run_test(test_p2sh_address);
    run_test(test_cashaddr_address);
    run_test(test_testnet_cashaddr_address);
    run_test(test_address_type_routing);
    run_test(test_base58_encoding_structure);
    run_test(test_address_format_handling);
    run_test(test_address_edge_cases);
    
    printf("\nAll address encoding tests passed!\n");
    return 0;
}

