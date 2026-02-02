#include "DirectoryNode.h"
#include "IVisitor.h"

void DirectoryNode::addComponent(std::shared_ptr<FileSystemComponent> component) {
	children.push_back(std::move(component));
}

const std::vector<std::shared_ptr<FileSystemComponent>>& DirectoryNode::getChildren() const {
	return children;
}

uint64_t DirectoryNode::getSize() const {
	return std::accumulate(children.begin(), children.end(), uint64_t(0),
		[](uint64_t total, const std::shared_ptr<FileSystemComponent>& child) {
			return total + child->getSize();
		});
}

bool DirectoryNode::isDirectory() const {
	return true;
}

void DirectoryNode::accept(IVisitor& visitor) {
	visitor.visitDirectory(*this);
}