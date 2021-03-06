/**
 * Copyright 2017-2020 Stefan Ascher
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "Registry.h"
#include "LogicConditions.h"
#include "FilterCondition.h"
#include "ZoneFilter.h"
#include "FirstFilter.h"
#include "LastFilter.h"
#include "HasSelection.h"
#include "Inverter.h"
#include "Succeed.h"
#include "Fail.h"
#include "Priority.h"
#include "Parallel.h"
#include "Sequence.h"
#include "Limit.h"
#include "Root.h"
#include "Repeater.h"
#include "UntilFail.h"
#include "UntilSuccess.h"

namespace AI {

Registry::Registry() = default;
Registry::~Registry() = default;

void Registry::Initialize()
{
    // Register default types
    RegisterNodeFactory("Priority", Priority::GetFactory());
    RegisterNodeFactory("Parallel", Parallel::GetFactory());
    RegisterNodeFactory("Sequence", Sequence::GetFactory());

    RegisterNodeFactory("Succeed", Succeed::GetFactory());
    RegisterNodeFactory("Fail", Fail::GetFactory());
    RegisterNodeFactory("Inverter", Inverter::GetFactory());
    RegisterNodeFactory("Limit", Limit::GetFactory());
    RegisterNodeFactory("Repeater", Repeater::GetFactory());
    RegisterNodeFactory("UntilFail", UntilFail::GetFactory());
    RegisterNodeFactory("UntilSuccess", UntilSuccess::GetFactory());

    RegisterConditionFactory("And", Conditions::AndCondition::GetFactory());
    RegisterConditionFactory("False", Conditions::FalseCondition::GetFactory());
    RegisterConditionFactory("Not", Conditions::NotCondition::GetFactory());
    RegisterConditionFactory("Or", Conditions::OrCondition::GetFactory());
    RegisterConditionFactory("True", Conditions::TrueCondition::GetFactory());
    RegisterConditionFactory("Filter", Conditions::FilterCondition::GetFactory());
    RegisterConditionFactory("HasSelection", Conditions::HasSelection::GetFactory());

    RegisterFilterFactory("Zone", Filters::ZoneFilter::GetFactory());
    RegisterFilterFactory("First", Filters::FirstFilter::GetFactory());
    RegisterFilterFactory("Last", Filters::LastFilter::GetFactory());
}

bool Registry::RegisterNodeFactory(const std::string& name, const NodeFactory& factory)
{
    return nodeFactory_.RegisterFactory(name, factory);
}

bool Registry::UnregisterNodeFactory(const std::string& name)
{
    return nodeFactory_.UnregisterFactory(name);
}

bool Registry::RegisterFilterFactory(const std::string& name, const FilterFactory& factory)
{
    return filterFactory_.RegisterFactory(name, factory);
}

bool Registry::UnregisterFilterFactory(const std::string& name)
{
    return filterFactory_.UnregisterFactory(name);
}

bool Registry::RegisterConditionFactory(const std::string& name, const ConditionFactory& factory)
{
    return conditionFactory_.RegisterFactory(name, factory);
}

bool Registry::UnregisterConditionFactory(const std::string& name)
{
    return conditionFactory_.UnregisterFactory(name);
}

std::shared_ptr<Node> Registry::CreateNode(const std::string& nodeType, const ArgumentsType& arguments)
{
    return nodeFactory_.Create(nodeType, arguments);
}

std::shared_ptr<Filter> Registry::CreateFilter(const std::string& filterType, const ArgumentsType& arguments)
{
    return filterFactory_.Create(filterType, arguments);
}

std::shared_ptr<Condition> Registry::CreateCondition(const std::string& conditionType, const ArgumentsType& arguments)
{
    return conditionFactory_.Create(conditionType, arguments);
}

}
