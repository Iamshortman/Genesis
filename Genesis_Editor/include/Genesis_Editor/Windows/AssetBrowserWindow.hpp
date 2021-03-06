#pragma once

#include <unordered_map>
#include "Genesis/Platform/FileSystem.hpp"
#include "Genesis/LegacyBackend/LegacyBackend.hpp"

namespace Genesis
{
	class AssetBrowserWindow
	{
	private:
		LegacyBackend* backend = nullptr;

		string project_directory;
		string active_directory;
		std::unordered_map<string, vector<FileInfo>> directory_map;
		
		Texture2D file_icon = nullptr;
		Texture2D directory_icon = nullptr;

		void refresh();
		void readDirectory(const string& directory_path);

		void drawDirectoryTree(const string& directory_path);
		void drawDirectoryPanel(const string& directory_path);

		void setProjectDirectory(const string& project_directory);

	public:
		AssetBrowserWindow(LegacyBackend* backend, const string& project_directory);
		~AssetBrowserWindow();

		void draw();
	};
}