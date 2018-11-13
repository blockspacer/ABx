/**
 * @file
 * @ingroup Filter
 */
#pragma once

#include "filter/IFilter.h"

namespace ai {

/**
 * @brief This filter is a nop - it will just use the already filtered entities.
 */
class SelectAll: public IFilter {
public:
	FILTER_CLASS_SINGLETON(SelectAll)

	void filter (const AIPtr& entity) override;
};

inline void SelectAll::filter (const AIPtr& /*entity*/) {
}

}
