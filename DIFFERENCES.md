# Differences Between CKPOOL-LHR and CKPOOL-LHR-BCH Fork

This document summarizes the key differences between the CKPOOL-LHR repository
and the CKPOOL-LHR-BCH fork.

## Overview

The CKPOOL-LHR-BCH fork is a specialized adaptation of CKPOOL-LHR for Bitcoin Cash (BCH) blockchain compatibility. It inherits all LHR features (fractional difficulty, user agent controls, etc.) while adding BCH-specific modifications.

## BCH-Specific Changes

### 1. Segwit Support Removal

**Purpose**: Bitcoin Cash does not support Segwit, so remove all Segwit-related code and RPC parameters.

**Changes**:
- `understood_rules[]` in `bitcoin.c` (line 20) changed from `{"segwit"}` to empty array `{}`
- `getblocktemplate` RPC request (line 117) removed `"rules": ["segwit"]` parameter
- `validate_address()` function (lines 85-90) updated to handle missing `iswitness` field gracefully
- Address validation no longer requires `iswitness` field from validateaddress RPC
- Witness commitment detection verified - BCH nodes don't return `default_witness_commitment`

**Safety**: Empty `understood_rules[]` array means pool will reject mining if BCH node requires any unknown consensus rules (prefixed with '!'). Since BCH returns NULL for rules field, no rules are enforced and mining proceeds safely.

**Technical Details**:
- Coinbase transactions will not include witness commitment output for BCH blocks
- `wb->insert_witness` remains false for all BCH block templates (stratifier.c:1358)
- Transaction output counts correctly exclude witness data (stratifier.c:451-453)
- `address_to_txn()` function always skips `segaddress_to_txn()` path for BCH addresses

### 2. Blockchain References

**Purpose**: Update all documentation and configuration to reference Bitcoin Cash instead of Bitcoin.

**Changes**:
- README.md updated to "Bitcoin Cash mining pool software"
- Installation scripts reference "Bitcoin Cash Core"
- Configuration examples use BCH-appropriate addresses
- Project name changed to CKPOOL-LHR-BCH

### 3. Node Compatibility

**Purpose**: Ensure compatibility with Bitcoin Cash nodes (Bitcoin ABC, etc.).

**Changes**:
- RPC calls remain compatible (getblocktemplate, validateaddress, etc.)
- No changes to core mining protocol logic
- Addresses use same format as Bitcoin (BCH uses legacy address format)

## Inherited LHR Features

CKPOOL-LHR-BCH inherits all features from CKPOOL-LHR:

### 1. Fractional Difficulty Support

**Purpose**: Enable fractional difficulty values (including sub-1.0) for low hash rate miners.

**Behavior**:
- All difficulty settings accept floating point values
- Values >= 1 rounded to nearest integer for emission
- Vardiff algorithm uses floating-point calculations
- Configuration validation with sensible defaults

### 2. User Agent Controls

**Purpose**: Optional whitelisting and aggregation of mining software user agents.

**Behavior**:
- Configurable whitelist using prefix matching
- User agent statistics exposed in pool status
- Normalization of common mining software identifiers

### 3. Enhanced Monitoring

**Purpose**: Improved operator visibility into pool operations.

**Behavior**:
- User agent usage statistics
- Enhanced logging and status endpoints
- Better error reporting for debugging

## Compatibility

- **Backward Compatible**: All CKPOOL-LHR configurations work unchanged
- **BCH Specific**: Optimized for Bitcoin Cash network parameters
- **Mining Software**: Compatible with all Stratum mining clients
- **Hardware**: Supports all mining hardware (ASIC, GPU, CPU, embedded)

## Migration from CKPOOL-LHR

1. Update `btcsig` in configuration to `"[:: BCH POOL ::]"`
2. Ensure Bitcoin Cash node is running (Bitcoin ABC recommended)
3. Update RPC connection to BCH node
4. Rebuild with `./configure && make`
5. Test with BCH testnet first if available

## Repository

- **Original**: https://github.com/Z3r0XG/ckpool-lhr
- **BCH Fork**: https://github.com/Z3r0XG/ckpool-lhr-bch
- **Upstream**: https://bitbucket.org/ckolivas/ckpool



