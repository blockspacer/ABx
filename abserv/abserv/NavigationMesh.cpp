#include "stdafx.h"
#include "NavigationMesh.h"

namespace Game {

static const int MAX_POLYS = 2048;

/// Temporary data for finding a path.
struct FindPathData
{
    // Polygons.
    dtPolyRef polys_[MAX_POLYS];
    // Polygons on the path.
    dtPolyRef pathPolys_[MAX_POLYS];
    // Points on the path.
    Math::Vector3 pathPoints_[MAX_POLYS];
    // Flags on the path.
    unsigned char pathFlags_[MAX_POLYS];
};

NavigationMesh::NavigationMesh() :
    AssetImpl<NavigationMesh>(),
    navMesh_(nullptr),
    navQuery_(nullptr),
    queryFilter_(nullptr),
    pathData_(std::make_unique<FindPathData>())
{
    navQuery_ = dtAllocNavMeshQuery();
}


NavigationMesh::~NavigationMesh()
{
    dtFreeNavMeshQuery(navQuery_);
    dtFreeNavMesh(navMesh_);
}

void NavigationMesh::FindPath(std::vector<Math::Vector3>& dest,
    const Math::Vector3& start, const Math::Vector3& end,
    const Math::Vector3& extends, const dtQueryFilter* filter)
{
    const dtQueryFilter* queryFilter = filter ? filter : queryFilter_.get();
    dtPolyRef startRef = 0;
    dtPolyRef endRef = 0;
    navQuery_->findNearestPoly(&start.x_, &extends.y_, queryFilter, &startRef, nullptr);
    navQuery_->findNearestPoly(&end.x_, &extends.y_, queryFilter, &startRef, nullptr);

    if (!startRef || !endRef)
        return;

    int numPolys = 0;
    int numPathPoints = 0;

    navQuery_->findPath(startRef, endRef, &start.x_, &end.x_, queryFilter,
        pathData_->polys_, &numPolys, MAX_POLYS);
    if (!numPolys)
        return;
}

}
