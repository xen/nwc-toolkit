// Copyright 2010 Susumu Yata <syata@acm.org>

#ifndef NWC_TOOLKIT_TOKEN_MAP_H_
#define NWC_TOOLKIT_TOKEN_MAP_H_

#include <tr1/unordered_map>
#include <vector>

#include "./string-hash.h"
#include "./string-pool.h"

namespace nwc_toolkit {

class TokenMap {
 public:
  TokenMap() : pool_(), map_(), tokens_() {}
  ~TokenMap() {
    Clear();
  }

  int operator[](const String &token) const {
    return map_.find(token)->second;
  }
  const String &operator[](int token_id) const {
    return tokens_[token_id];
  }

  std::size_t num_tokens() const {
    return tokens_.size();
  }

  void Clear() {
    pool_.Clear();
    map_.clear();
    tokens_.clear();
  }

  int Insert(const String &token) {
    MapType::iterator it = map_.find(token);
    if (it != map_.end()) {
      return it->second;
    }
    int token_id = static_cast<int>(num_tokens());
    String token_clone = pool_.Append(token);
    map_[token_clone] = token_id;
    tokens_.push_back(token_clone);
    return token_id;
  }

 private:
  typedef std::tr1::unordered_map<String, int, StringHash> MapType;

  StringPool pool_;
  MapType map_;
  std::vector<String> tokens_;

  // Disallows copy and assignment.
  TokenMap(const TokenMap &);
  TokenMap &operator=(const TokenMap &);
};

}  // namespace nwc_toolkit

#endif  // NWC_TOOLKIT_TOKEN_MAP_H_
