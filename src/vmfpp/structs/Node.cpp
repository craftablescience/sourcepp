#include <vmfpp/structs/Node.h>

using namespace vmfpp;

const std::unordered_map<std::string, std::vector<std::string>>& Node::getValues() const {
	return this->values;
}

std::unordered_map<std::string, std::vector<std::string>>& Node::getValues() {
	return this->values;
}

bool Node::hasValue(const std::string& key) const {
	return this->values.contains(key);
}

const std::vector<std::string>& Node::getValue(const std::string& key) const {
	return this->values.at(key);
}

std::vector<std::string>& Node::getValue(const std::string& key) {
	return this->values.at(key);
}

void Node::addValue(std::string key, std::string value) {
	if (!this->values.contains(key)) {
		this->values[key] = {};
	}
	this->values[std::move(key)].push_back(std::move(value));
}

void Node::removeValue(const std::string& key) {
	this->values.erase(key);
}

const std::unordered_map<std::string, std::vector<Node>>& Node::getChildren() const {
	return this->children;
}

std::unordered_map<std::string, std::vector<Node>>& Node::getChildren() {
	return this->children;
}

bool Node::hasChild(const std::string& key) const {
	return this->children.contains(key);
}

const std::vector<Node>& Node::getChild(const std::string& key) const {
	return this->children.at(key);
}

std::vector<Node>& Node::getChild(const std::string& key) {
	return this->children.at(key);
}

void Node::addChild(std::string key, Node value) {
	if (!this->children.contains(key)) {
		this->children[key] = {};
	}
	this->children.at(key).push_back(std::move(value));
}

void Node::removeChild(const std::string& key) {
	this->children.erase(key);
}

std::size_t Node::getNumItems() const {
	return this->values.size() + this->children.size();
}
