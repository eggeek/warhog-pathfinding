#pragma once

#include <vector>
#include "search_node.h"
#include "constants.h"

namespace warthog {

using namespace std;
class online_jps_pruner {
  public:
    typedef pair<uint32_t, warthog::cost_t> pic; // Pair of <Id, Cost>
    bool rmapflag;
    uint32_t scan_cnt;
    search_node* cur;
    vector<pic> vis;

    uint32_t curg;
    uint32_t jumplimit_;
    uint32_t jlimith, jlimitv;
    uint32_t jumpdist;
    enum EndType {
      forced,     // ended at a forced neighbour
      deadend,    // ended at a deadend
      pruned,     // ended by pruning strategy
      reached     // ended at target
    } etype;

    inline void init(uint32_t tot) {
      vis.resize(tot);
      fill(vis.begin(), vis.end(), pic{warthog::INF, warthog::INF});
      scan_cnt = 0;
    }

    inline void set_forced() { this->etype = forced; }
    inline void set_pruned() { this->etype = pruned; }
    inline void set_deadend() { this->etype = deadend; }
    inline void set_reached() { this->etype = reached; }
    inline bool is_forced() { return this->etype == forced; }
    inline bool is_pruned() { return this->etype == pruned; }
    inline bool is_deadend() { return this->etype == deadend; }
    inline bool is_reached() { return this->etype == reached; }

    inline bool gValPruned(const uint32_t& jumpnode_id, const uint32_t& goal_id,const warthog::cost_t& c) {

      if (vis[jumpnode_id].first != goal_id || vis[jumpnode_id].second == warthog::INF) {
        vis[jumpnode_id] = {goal_id, c};
        return false;
      }
      else {
        if (c > vis[jumpnode_id].second) {
          this->set_pruned();
          return true;
        }
        else {
          vis[jumpnode_id] = {goal_id, c};
          return false;
        }
      }
    }

    inline uint32_t gVal(const uint32_t& jumpnode_id, const uint32_t& goal_id) {
      if (vis[jumpnode_id].first != goal_id) return warthog::INF;
      else return vis[jumpnode_id].second;
    }

    inline void update_jlimtv(const uint32_t& jumpnode_id) { 
      switch (this->etype) {
        case pruned:
          this->jlimitv = this->jumpdist-1; break;
        case reached:
          this->jlimitv = 0; break;
        case deadend:
          this->jlimitv = warthog::INF; break;
        case forced:
          this->jlimitv = this->jumpdist-1; break;
        default: break;
      }
    }

    inline void update_jlimth(const uint32_t& jumpnode_id) {
      switch (this->etype) {
        case pruned:
          this->jlimith = this->jumpdist-1; break;
        case reached:
          this->jlimith = 0; break;
        case deadend:
          this->jlimith = warthog::INF; break;
        case forced:
          this->jlimith = this->jumpdist-1; break;
        default: break;
      }
    }

    inline void startJump(search_node* cur) {
      this->cur = cur;
      this->curg = cur->get_g();
    }

    inline void startExpand() {
      this->jumpdist = this->jlimith = this->jlimitv = warthog::INF;
      this->set_forced();
      this->rmapflag = false;
    }
};
}
