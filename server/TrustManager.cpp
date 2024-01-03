// TrustManager.cpp
#include "TrustManager.h"


// Declaration of the global trustMap as extern.
std::unordered_map<std::pair<std::string, salticidae::PeerId>, TrustInfo, pair_hash> trustMap;