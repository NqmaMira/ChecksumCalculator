#include "FileNode.h"
#include "IVisitor.h"

FileNode::FileNode(std::string name, std::string path, uint64_t size)
	: FileSystemComponent(std::move(name), std::move(path)), size(size) { }

uint64_t FileNode::getSize() const {
	return size;
}

bool FileNode::isDirectory() const {
	return false;
}

void FileNode::accept(IVisitor& visitor) {
	visitor.visitFile(*this);
}

void FileNode::setHash(const std::string& hashValue) {
	hash = hashValue;
}

std::string FileNode::getHash() const {
	return hash;
}