// Copyright (c) 2017-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_RPC_BLOCKCHAIN_H
#define BITCOIN_RPC_BLOCKCHAIN_H

#include <consensus/amount.h>
#include <core_io.h>
#include <fs.h>
#include <span.h>
#include <streams.h>
#include <sync.h>
#include <coins.h>

#include <any>
#include <optional>
#include <stdint.h>
#include <vector>

extern RecursiveMutex cs_main;

class CBlock;
class CBlockIndex;
class CChainState;
class ChainstateManager;
class CTxMemPool;
class UniValue;
namespace node {
struct NodeContext;
} // namespace node

static constexpr int NUM_GETBLOCKSTATS_PERCENTILES = 5;
using coinascii_cb_t = std::function<std::string(const COutPoint&, const Coin&)>;

/**
 * Get the difficulty of the net wrt to the given block index.
 *
 * @return A floating point number that is a multiple of the main net minimum
 * difficulty (4295032833 hashes).
 */
double GetDifficulty(const CBlockIndex* blockindex);

/** Callback for when block tip changed. */
void RPCNotifyBlockChange(const CBlockIndex*);

/** Block description to JSON */
UniValue blockToJSON(const CBlock& block, const CBlockIndex* tip, const CBlockIndex* blockindex, TxVerbosity verbosity) LOCKS_EXCLUDED(cs_main);

typedef std::vector<CAmount> MempoolHistogramFeeRates;

/* TODO: define log scale formular for dynamically creating the
 * feelimits but with the property of not constantly changing
 * (and thus screw up client implementations) */
static const MempoolHistogramFeeRates MempoolInfoToJSON_const_histogram_floors{
    1, 2, 3, 4, 5, 6, 7, 8, 10,
    12, 14, 17, 20, 25, 30, 40, 50, 60, 70, 80, 100,
    120, 140, 170, 200, 250, 300, 400, 500, 600, 700, 800, 1000,
    1200, 1400, 1700, 2000, 2500, 3000, 4000, 5000, 6000, 7000, 8000, 10000};

/** Mempool information to JSON */
UniValue MempoolInfoToJSON(const CTxMemPool& pool, const std::optional<MempoolHistogramFeeRates>& histogram_floors);

/** Mempool to JSON */
UniValue MempoolToJSON(ChainstateManager& chainman, const CTxMemPool& pool, bool verbose = false, bool include_mempool_sequence = false);

/** Block header to JSON */
UniValue blockheaderToJSON(const CBlockIndex* tip, const CBlockIndex* blockindex) LOCKS_EXCLUDED(cs_main);

/** Used by getblockstats to get feerates at different percentiles by weight  */
void CalculatePercentilesByWeight(CAmount result[NUM_GETBLOCKSTATS_PERCENTILES], std::vector<std::pair<CAmount, int64_t>>& scores, int64_t total_weight);

/**
 * Helper to create UTXO snapshots given a chainstate and a file handle.
 * @return a UniValue map containing metadata about the snapshot.
 */
UniValue CreateUTXOSnapshot(
    const bool is_human_readable,
    const bool show_header,
    const Span<const std::byte>& separator,
    const std::vector<std::pair<std::string, coinascii_cb_t>>& requested,
    node::NodeContext& node,
    CChainState& chainstate,
    CAutoFile& afile,
    const fs::path& path,
    const fs::path& tmppath);

#endif // BITCOIN_RPC_BLOCKCHAIN_H
