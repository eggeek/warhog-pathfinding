#include "jps_expansion_policy.h"
#include "global.h"
namespace G = global;

warthog::jps_expansion_policy::jps_expansion_policy(warthog::gridmap* map)
{
	map_ = map;
	nodepool_ = new warthog::blocklist(map->height(), map->width());
  G::query::nodepool = nodepool_;
	jpl_ = new warthog::online_jump_point_locator(map);
	reset();
}

warthog::jps_expansion_policy::~jps_expansion_policy()
{
	delete jpl_;
	delete nodepool_;
}

void 
warthog::jps_expansion_policy::expand(
		warthog::search_node* current, warthog::problem_instance* problem)
{
	reset();

#ifdef CNT 
  G::statis::update_subopt_expd(current->get_id(), current->get_g());
#endif

  uint32_t searchid = problem->get_searchid();
	// compute the direction of travel used to reach the current node.
	warthog::jps::direction dir_c =
	   	this->compute_direction(current->get_parent(), current);

	// get the tiles around the current node c
	uint32_t c_tiles;
	uint32_t current_id = current->get_id();
	map_->get_neighbours(current_id, (uint8_t*)&c_tiles);

	// look for jump points in the direction of each natural 
	// and forced neighbour
	uint32_t succ_dirs = warthog::jps::compute_successors(dir_c, c_tiles);
	uint32_t goal_id = problem->get_goal();
	for(uint32_t i = 0; i < 8; i++)
	{
		warthog::jps::direction d = (warthog::jps::direction) (1 << i);
		if(succ_dirs & d)
		{
			warthog::cost_t jumpcost;
			uint32_t succ_id;
			jpl_->jump(d, current_id, goal_id, succ_id, jumpcost);

			if(succ_id != warthog::INF)
			{

		    warthog::search_node* mynode = nodepool_->generate(succ_id);
        if (mynode->get_searchid() != searchid) {
          mynode->reset(searchid);
        }
        mynode->set_pdir(d);

#ifdef CNT
        G::statis::update_subopt_touch(mynode->get_id(), current->get_g()+jumpcost);
#endif
				neighbours_[num_neighbours_] = mynode;
				costs_[num_neighbours_] = jumpcost;
				// move terminator character as we go
				neighbours_[++num_neighbours_] = 0;
			}
		}
	}
}

