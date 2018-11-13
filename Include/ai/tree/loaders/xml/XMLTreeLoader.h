/**
 * @file
 */
#pragma once

#include "tree/loaders/ITreeLoader.h"
#include <tinyxml2.h>
#include "conditions/ConditionParser.h"
#include "tree/TreeNodeParser.h"
#include "tree/TreeNodeImpl.h"

namespace ai {

/**
 * @brief Implementation of @c ITreeLoader that gets its data from a xml file.
 */
class XMLTreeLoader: public ai::ITreeLoader {
private:
	TreeNodePtr loadSubTreeFromXML (const IAIFactory& aiFactory, tinyxml2::XMLElement* e) {
		if (e == nullptr) {
			return TreeNodePtr();
		}

		const char *name = e->Attribute("name", nullptr);
		if (name == nullptr) {
			setError("No name given");
			return TreeNodePtr();
		}

		const char *type = e->Attribute("type", nullptr);
		if (type == nullptr) {
			setError("No type given for name %s", name);
			return TreeNodePtr();
		}

		TreeNodeParser nodeParser(aiFactory, type);
		const TreeNodePtr& node = nodeParser.getTreeNode(name);
		if (!node) {
			setError("Could not create the tree node for name %s (type: %s)", name, type);
			return TreeNodePtr();
		}

		const char *condition = e->Attribute("condition", nullptr);
		if (condition == nullptr) {
			condition = "True";
		}

		ConditionParser conditionParser(aiFactory, condition);
		const ConditionPtr& conditionPtr = conditionParser.getCondition();
		if (!conditionPtr.get()) {
			setError("Could not create the condition for %s (name %s, type: %s)", condition, name, type);
			return TreeNodePtr();
		}

		node->setCondition(conditionPtr);
		return node;
	}

	TreeNodePtr loadTreeFromXML (const IAIFactory& aiFactory, tinyxml2::XMLElement* rootNode) {
		TreeNodePtr root = loadSubTreeFromXML(aiFactory, rootNode);
		if (!root.get()) {
			return root;
		}
		for (tinyxml2::XMLNode* node = rootNode->FirstChild(); node; node = node->NextSibling()) {
			tinyxml2::XMLElement* e = node->ToElement();
			const TreeNodePtr& child = loadSubTreeFromXML(aiFactory, e);
			if (child.get() == nullptr) {
				continue;
			}
			root->addChild(child);
		}
		return root;
	}

public:
	explicit XMLTreeLoader(const IAIFactory& aiFactory) :
			ITreeLoader(aiFactory) {
	}

	/**
	 * @brief this will initialize the loader once with all the defined behaviours from the given xml data.
	 */
	bool init(const std::string& xmlData, const char* rootNodeName = "trees", const char *treeNodeName = "tree") {
		resetError();
		tinyxml2::XMLDocument doc(false);
		const int status = doc.Parse(xmlData.c_str());
		tinyxml2::XMLElement* rootNode = doc.FirstChildElement(rootNodeName);
		if (rootNode == nullptr) {
			return false;
		}
		for (tinyxml2::XMLNode* node = rootNode->FirstChild(); node; node = node->NextSibling()) {
			tinyxml2::XMLElement* e = node->ToElement();
			if (e == nullptr) {
				setError("unexpected node type");
				continue;
			}
			if (e->Name() == nullptr) {
				setError("expected node name but didn't find one");
				continue;
			}
			if (::strcmp(treeNodeName, e->Name())) {
				setError("unexpected node name - expected 'tree' - got %s", e->Name());
				continue;
			}
			const char *name = e->Attribute("name");
			if (name == nullptr) {
				setError("node 'tree' does not have a 'name' attribute");
				continue;
			}
			tinyxml2::XMLNode* rootXMLNode = e->FirstChild();
			if (rootXMLNode == nullptr) {
				setError("node 'tree' doesn't have a child (which should e.g. be a selector)");
				continue;
			}
			const TreeNodePtr& root = loadTreeFromXML(_aiFactory, rootXMLNode->ToElement());
			if (root.get() == nullptr) {
				setError("could not create the root node");
				continue;
			}
			addTree(name, root);
		}
		if (status != tinyxml2::XML_NO_ERROR) {
			return false;
		}
		return getError().empty();
	}
};

}
