#include "stdafx.h"
#include "Composite.h"

namespace AI {

Composite::Composite(const ArgumentsType& arguments) :
    Node(arguments)
{ }

bool Composite::AddNode(std::shared_ptr<Node> node)
{
    if (!node)
        return false;
    children_.push_back(node);
    return true;
}

}