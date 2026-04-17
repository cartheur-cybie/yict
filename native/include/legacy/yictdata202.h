#pragma once

// Reconstructed from ../himage/src/yictdata.a_ and legacy source usage in
// source/cart.cpp + source/custom.cpp. This layout is used by the native
// Linux port as the canonical in-memory representation for YICT202 metadata.

#include <cstdint>

namespace yict::legacy {

using addr_t = std::uint32_t;

constexpr std::uint32_t CART_BASE = 0x200000;
constexpr std::uint32_t CART_SIZE = 256 * 1024;
constexpr std::uint32_t ROM_HALF_SIZE = CART_SIZE / 2;
constexpr std::uint32_t YICT_CUSTOMIZATION_ADDRESS = 0x23FE00;
constexpr std::uint32_t YICT_CUSTOMIZATION_OFFSET = YICT_CUSTOMIZATION_ADDRESS - CART_BASE;
constexpr std::size_t YICTTAG_SIZE = 8;

constexpr const char YICT_ID[] = "YICT202";
constexpr const char YICTEND_ID[] = "YICTEND";

constexpr int NUM_AUDIO_POOLS = 3;
constexpr int NUM_ACTION_CHUNK = 3;

#pragma pack(push, 1)
struct YICT_MEMPOOL {
  addr_t start;
  std::uint16_t max_size;
};

struct YICT_ACTIONCHUNK {
  addr_t start;
  std::uint16_t count;
};

struct YICT_DATA {
  char yict_id[YICTTAG_SIZE];
  char copyright[24];
  std::uint16_t generation;

  YICT_MEMPOOL pools[NUM_AUDIO_POOLS];

  addr_t addrSysAudioLookup;
  addr_t addrExtraAudioPointers;

  addr_t addrProbActionTable;
  addr_t addrExtraActionTable;
  std::uint16_t numExtraActionTable;

  addr_t addrSitStandNewData;

  YICT_ACTIONCHUNK availableActions[NUM_ACTION_CHUNK];

  char name[16];
  char basename[16];

  std::uint16_t audio_sizes[NUM_AUDIO_POOLS];
  std::uint16_t pool3_yictsize;

  std::uint8_t bStretchCount;
  std::uint8_t bWagTime;
  std::uint16_t wSslSkipCount;

  std::uint8_t mood_data[10];

  std::uint8_t boolDisableCharger;
  std::uint8_t boolMute;
  std::uint8_t bIdleDelay;
  std::uint8_t reserved;

  char end_tag[YICTTAG_SIZE];
};
#pragma pack(pop)

}  // namespace yict::legacy
