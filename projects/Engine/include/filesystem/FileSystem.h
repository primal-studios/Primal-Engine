#ifndef filesystem_h__
#define filesystem_h__

#include <cstdint>

#include <vector>
#include <string>

#include "filesystem/File.h"
#include "core/Types.h"

struct FileInfo
{
	std::string stem;
	std::string extension;
	std::string filename;

	std::string parentPath;
	std::string rootPath;
};

class FileSystem
{
	public:
		static FileSystem& instance();

		void mount(const Path& aPath);
		void unmount();

		Path getMountedDirectory() const;

		File* load(const Path& aPath) const;
		std::string loadToString(const Path& aPath) const;
		std::vector<char> getBytes(const Path& aPath) const;

		bool exists(const Path& aPath) const;

		bool isFile(const Path& aPath) const;
		bool isDirectory(const Path& aPath) const;

		File* create(const std::string& aPath) const;
		void createDirectory(const Path& aPath) const;

		std::vector<FileInfo> getFilesInPath(const Path& aPath, const bool aRecursive = false) const;

	private:
		FileSystem();

		Path mMountedPath;
};

#endif // filesystem_h__