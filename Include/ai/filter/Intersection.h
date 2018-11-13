/**
 * @file
 * @ingroup Filter
 * @image html intersection.png
 */
#pragma once

#include "filter/IFilter.h"
#include <algorithm>

namespace ai {

/**
 * @brief This filter performs an intersection between several filter results
 */
class Intersection: public IFilter {
public:
	FILTER_ACTION_CLASS(Intersection)
	FILTER_ACTION_FACTORY(Intersection)

	void filter (const AIPtr& entity) override;
};

inline void Intersection::filter (const AIPtr& entity) {
	FilteredEntities& filtered = getFilteredEntities(entity);
	// create a copy
	const FilteredEntities alreadyFiltered = filtered;
	// now clear the entity list
	filtered.clear();

	std::vector<FilteredEntities> filteredArray(_filters.size());
	int n = 0;
	size_t max = 0u;
	for (auto& f : _filters) {
		f->filter(entity);
		filteredArray[n++] = filtered;
		max = std::max(filtered.size(), max);
		// safe and clear
		filtered.clear();
	}

	for (size_t i = 0; i < filteredArray.size(); ++i) {
		std::sort(filteredArray[i].begin(), filteredArray[i].end());
	}

	FilteredEntities result(max);
	std::set_intersection(
			filteredArray[0].begin(), filteredArray[0].end(),
			filteredArray[1].begin(), filteredArray[1].end(),
			std::back_inserter(result));

	if (filteredArray.size() >= 2) {
		FilteredEntities buffer(max);
		for (size_t i = 2; i < filteredArray.size(); ++i) {
			buffer.clear();
			std::sort(result.begin(), result.end());
			std::set_intersection(
					result.begin(), result.end(),
					filteredArray[i].begin(), filteredArray[i].end(),
					std::back_inserter(buffer));
			std::swap(result, buffer);
		}
	}

	filtered.reserve(alreadyFiltered.size() + max);
	for (auto& e : alreadyFiltered) {
		filtered.push_back(e);
	}
	for (auto& e : result) {
		filtered.push_back(e);
	}
}

}
