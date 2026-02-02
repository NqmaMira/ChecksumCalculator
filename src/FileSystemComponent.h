#pragma once
#include <string>
#include <vector>
#include <memory>

class FileSystemComponent {
protected:
    std::string name;
    std::string path;

public:
    FileSystemComponent(std::string name, std::string path)
        : name(std::move(name)), path(std::move(path)) { }

    virtual ~FileSystemComponent() = default;

    virtual std::string getName() const { return name; }
    virtual std::string getPath() const { return path; }

    virtual uint64_t getSize() const = 0;

    virtual bool isDirectory() const = 0;
};