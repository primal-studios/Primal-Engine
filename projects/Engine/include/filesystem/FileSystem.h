#ifndef filesystem_h__
#define filesystem_h__

#include <cstdint>

#include <vector>
#include <string>

#include "filesystem/File.h"

struct FileInfo
{
	std::string name;
	std::string path;
	size_t size;
};

class FileSystem
{
	public:
		static FileSystem& instance();

		void mount(const std::string& aPath);
		void unmount();

		File* load(const std::string& aPath) const;
		std::string loadToString(const std::string& aPath) const;

		bool exists(const std::string& aPath) const;

		File* create(const std::string& aPath) const;
		void createDirectory(const std::string& aPath) const;

		std::vector<FileInfo> getAllFilesInDirectory(const std::string& aPath, const bool aRecursive = false) const;

	private:
		FileSystem();

		std::string mMountedPath;
};

#endif // filesystem_h__