#pragma once

#include "base.h"
#include "options.h"
#include "list.h"

namespace _STD_BUILD {
	namespace __cache {

		const std::string cache_file_id{ "STDBUILD" };

		namespace serialization {

			void write_string(std::ofstream& stream, const std::string& str) {
				const std::size_t size = str.size();
				stream.write(reinterpret_cast<const char*>(&size), sizeof(std::size_t));
				stream.write(str.data(), static_cast<std::streamsize>(size));
			}

			void read_string(std::ifstream& stream, std::string& str) {
				std::streamsize size{ 0 };
				stream.read(reinterpret_cast<char*>(&size), sizeof(std::size_t));
				str.resize(static_cast<std::size_t>(size));
				stream.read(str.data(), size);
			}

			void write_int64_t(std::ofstream& stream, const int64_t i) { stream.write(reinterpret_cast<const char*>(&i), sizeof(int64_t)); }
			void read_int64_t(std::ifstream& stream, int64_t& i) { stream.read(reinterpret_cast<char*>(&i), sizeof(int64_t)); }
			void write_size_t(std::ofstream& stream, const std::size_t i) { stream.write(reinterpret_cast<const char*>(&i), sizeof(std::size_t)); }
			void read_size_t(std::ifstream& stream, std::size_t& i) { stream.read(reinterpret_cast<char*>(&i), sizeof(std::size_t)); }

		} // namespace serialization

		// Returns the file's last write time since epoch in seconds
		inline int64_t get_lwt(const fs::path& file) noexcept {
			const auto lwt = fs::last_write_time(file);

			// This two methods of converting lwt to an integer produce very different results.
			// The first one that only works after C++20 produces the correct result.
			// The second that works on C++17 produces a passable result, but is negative
			// and does not relate to the correct epoch time.
#if __cpp_lib_chrono >= 201907L
			return std::chrono::system_clock::to_time_t(std::chrono::clock_cast<std::chrono::system_clock>(lwt));
#elif __cpp_lib_chrono >= 201611L
			return std::chrono::duration_cast<std::chrono::seconds>(lwt.time_since_epoch()).count();
#else
			return 0; // TODO: Stop compilation here.
#endif
		}

		struct lwt_file_block {
			fs::path path;
			int64_t lwt{ 0 };
			path_list dependent_files;

			lwt_file_block() = default;
			lwt_file_block(const fs::path _path) : path(_path) { lwt = get_lwt(_path); }
			lwt_file_block(const fs::path _path, int64_t _lwt) : path(_path), lwt(_lwt) {}
			lwt_file_block(const fs::path _path, int64_t _lwt, const path_list& _dependent_files)
			    : path(_path), lwt(_lwt), dependent_files(_dependent_files) {}

			void reload_lwt() { lwt = get_lwt(path); }

			operator ListElement<fs::path>() { return ListElement<fs::path>{ path }; }
			bool operator==(const ListElement<fs::path>& file) { return (path == file.value); }
			friend bool operator==(const ListElement<fs::path>& file, const lwt_file_block& lwtfb) { return (file.value == lwtfb.path); }
			bool operator<(const ListElement<fs::path>& file) { return (path < file.value); }
			friend bool operator<(const ListElement<fs::path>& file, const lwt_file_block& lwtfb) { return (file.value < lwtfb.path); }

			friend std::ofstream& operator<<(std::ofstream& stream, const lwt_file_block& block) {
				serialization::write_string(stream, block.path.string());
				serialization::write_int64_t(stream, block.lwt);

				// write the source files that depend on this header
				serialization::write_size_t(stream, block.dependent_files.size());

				// iterate through list of dependent files and write their paths out
				for(const auto& source : block.dependent_files) {
					serialization::write_string(stream, source.value.string());
				}

				return stream;
			}

			friend std::ifstream& operator>>(std::ifstream& stream, lwt_file_block& block) {
				// read the path
				std::string temp_path;
				serialization::read_string(stream, temp_path);
				block.path = { temp_path };
				// read the "last write time"
				serialization::read_int64_t(stream, block.lwt);

				// read in the number of source files for this header
				std::size_t source_file_count{ 0 };
				serialization::read_size_t(stream, source_file_count);
				block.dependent_files.get().reserve(source_file_count);

				for(std::size_t i = 0; i < source_file_count; ++i) {
					std::string source_path;
					serialization::read_string(stream, source_path);
					block.dependent_files.get().push_back({ source_path });
				}

				return stream;
			}
		};

		template <typename _InIt, typename _InIt2, typename _OutIt>
		void find_differences(const _InIt begin1, const _InIt end1, const _InIt2 begin2, const _InIt2 end2, _OutIt add, _OutIt sub) {
			for(_InIt temp = begin1; temp != end1; ++temp) {
				if(std::find(begin2, end2, *temp) == end2) {
					*add++ = *temp;
				}
			}

			for(_InIt2 temp = begin2; temp != end2; ++temp) {
				if(std::find(begin1, end1, *temp) == end1) {
					*sub++ = *temp;
				}
			}
		}

		struct cache_storage {
		private:
			struct cache_changes {
				path_list added, removed, modified;
			};

		public:
			std::vector<lwt_file_block> source_files;
			std::vector<lwt_file_block> header_files;
			cache_changes changes;

			void write_to_file(const fs::path& file_path) {
				std::ofstream cache_file(file_path.string(), std::ios::binary);
				if(cache_file.is_open()) {
					// Write file identification code.
					serialization::write_string(cache_file, cache_file_id);
					cache_file << *this;
					cache_file.close();
				}
			}

			bool load_from_file(const fs::path& file_path) {
				std::ifstream cache_file(file_path.string(), std::ios::binary);
				if(cache_file.is_open()) {

					// Read and verify identification code.
					std::string id;
					serialization::read_string(cache_file, id);
					if(id != cache_file_id) {
						return false;
					}

					cache_file >> *this;

					return true;
					cache_file.close();
				}
				return false;
			}

			void check_for_file_changes() {
				auto& data = changes.modified.get();

				for(const auto& source : source_files) {
					if(get_lwt(source.path) > source.lwt) {
						data.push_back(source.path);
					}
				}

				for(const auto& header : header_files) {
					if(get_lwt(header.path) > header.lwt && header.dependent_files.size() > 0) {
						data.insert(data.end(), header.dependent_files.begin(), header.dependent_files.end());
					}
				}

				// Sort and remove any duplicates
				std::sort(data.begin(), data.end());
				data.erase(std::unique(data.begin(), data.end()), data.end());
			}

			// TODO: This is horrible. Clean it up.
			//  To update the cache we need to deal with the following: modified/added/removed source files.
			void update(const path_list& include_dirs) {
				for(const auto& modded_file : changes.modified) {
					// Verify that the modded file does exist in our cache source file list.
					const auto it = std::find(source_files.begin(), source_files.end(), modded_file);
					if(it != source_files.end()) {
						// If the file is present, then rebuild the include hierarchy.
						(*it).dependent_files = get_includes(include_dirs, (*it).path);
						(*it).reload_lwt();

						// We need to make sure that all the header files have this source file as one of their
						// dependents.
						for(const auto& dep_header : (*it).dependent_files) {
							// Check if this header already exists in the cache header list.
							auto it2 = std::find(header_files.begin(), header_files.end(), dep_header);
							if(it2 != header_files.end()) {
								// If it does then check if this header has the source in its dependency list.
								auto it3 = std::find((*it2).dependent_files.begin(), (*it2).dependent_files.end(), modded_file);
								if(it3 == (*it2).dependent_files.end()) {
									// File doesn't exist in the headers dependency list, so add it.
									(*it2).dependent_files.add(modded_file);
								}
							} else {
								header_files.push_back({ dep_header.value, get_lwt(dep_header.value), { modded_file } });
							}
						}
					}
				}

				// For each of the newly added source files:
				for(const auto& new_file : changes.added) {
					// Verify whether the modded file exists in our cache source file list.
					const auto it = std::find(source_files.begin(), source_files.end(), new_file);
					if(it != source_files.end()) {
						// If the file is present, then rebuild the include hierarchy.
						(*it).dependent_files = get_includes(include_dirs, (*it).path);
						(*it).reload_lwt();
					} else {
						// If not, then add it to the source list.
						source_files.push_back({ new_file.value });
					}
				}

				// For every source file that was removed:
				for(const auto& removed_file : changes.removed) {
					// Check to see if it exists in the cache source file list.
					const auto it = std::find(source_files.begin(), source_files.end(), removed_file);
					if(it != source_files.end()) {
						// If it exists, iterate through the dependent headers and remove the source file from their
						// depednency lists
						for(const auto& header : (*it).dependent_files) {
							const auto it2 = std::find(header_files.begin(), header_files.end(), header);
							if(it2 != header_files.end()) {
								const auto it3 = std::find((*it2).dependent_files.begin(), (*it2).dependent_files.end(), removed_file);
								if(it3 != (*it2).dependent_files.end()) {
									(*it2).dependent_files.get().erase(it3);
								}
							}
						}
						source_files.erase(it);
					}
				}
			}

			void test(const path_list& cpp_files) {
				find_differences(cpp_files.begin(), cpp_files.end(), source_files.begin(), source_files.end(),
				                 std::back_inserter(changes.added.get()), std::back_inserter(changes.removed.get()));
				check_for_file_changes();
			}

			friend std::ofstream& operator<<(std::ofstream& stream, const cache_storage& cache) {
				// Write source file last write times
				serialization::write_size_t(stream, cache.source_files.size());
				for(const auto& source_file : cache.source_files) {
					stream << source_file;
				}
				// Write header file last write times
				serialization::write_size_t(stream, cache.header_files.size());
				for(const auto& header_file : cache.header_files) {
					stream << header_file;
				}

				return stream;
			}

			friend std::ifstream& operator>>(std::ifstream& stream, cache_storage& cache) {
				// Read in the source files
				std::size_t source_file_count{ 0 };
				serialization::read_size_t(stream, source_file_count);
				cache.source_files.reserve(source_file_count);
				for(std::size_t i = 0; i < source_file_count; ++i) {
					lwt_file_block temp;
					stream >> temp;
					cache.source_files.push_back(std::move(temp));
				}

				// Read in the header files
				std::size_t header_file_count{ 0 };
				serialization::read_size_t(stream, header_file_count);
				cache.header_files.reserve(header_file_count);
				for(std::size_t i = 0; i < header_file_count; ++i) {
					lwt_file_block temp;
					stream >> temp;
					cache.header_files.push_back(std::move(temp));
				}

				return stream;
			}

			friend std::ostream& operator<<(std::ostream& stream, const cache_storage& cache) {
				stream << "\n------------------------------\n";
				stream << "Cache:\n";
				stream << "Source files:\n";
				for(const auto& source_file : cache.source_files) {
					stream << '\t' << source_file.path.string() << '\n';
					for(const auto& dep : source_file.dependent_files) {
						stream << "\t\t" << dep.value.string() << '\n';
					}
				}
				stream << "Header files:\n";
				for(const auto& header_file : cache.header_files) {
					stream << '\t' << header_file.path.string() << '\n';
					for(const auto& dep : header_file.dependent_files) {
						stream << "\t\t" << dep.value.string() << '\n';
					}
				}

				stream << "Added:\n";
				for(const auto& added : cache.changes.added) {
					stream << '\t' << added << '\n';
				}

				stream << "Removed:\n";
				for(const auto& removed : cache.changes.removed) {
					stream << '\t' << removed << '\n';
				}

				stream << "Modified:\n";
				for(const auto& modified : cache.changes.modified) {
					stream << '\t' << modified << '\n';
				}
				stream << "------------------------------\n";
				return stream;
			}
		};

		cache_storage build(const path_list& include_directories, const path_list& cpp_files) {
			std::unordered_map<fs::path, path_list> cpp_file_dependencies;
			for(const auto& file : cpp_files) {
				// Iterate through every file and verify it exists.
				if(fs::exists(file.value)) {
					// Clean up the slashes uses lexically normal.
					const auto lexically_normal_file_path = file.value.lexically_normal();

					// Recursively get all the header files associated with this source file and add them to the source
					// files dependents list.
					cpp_file_dependencies[lexically_normal_file_path] = get_includes(include_directories, lexically_normal_file_path);
				} else {
					_STD_BUILD_OUTPUT("File " << file.value << " was not found and cannot be added to the cache.\n");
				}
			}

			// Invert the dependency tree, so that we have a list of header files that contains the source files that
			// depend on them. By checking for a change in a header, we can know which source files need to be
			// recompiled because of that change.
			std::unordered_map<fs::path, path_list> headers;

			for(const auto& [cpp_file, dependencies] : cpp_file_dependencies) {
				for(const auto& dep : dependencies) {
					auto& hv = headers[dep.value];
					if(std::find(hv.begin(), hv.end(), cpp_file) == hv.end()) {
						hv.get().push_back(cpp_file);
					}
				}
			}

			cache_storage cache;

			for(const auto& source : cpp_files) {
				cache.source_files.push_back({ source.value.lexically_normal() });
			}

			for(const auto& header : headers) {
				cache.header_files.push_back({ header.first, get_lwt(header.first), header.second });
			}

			return cache;
		}

		inline bool cache_exists(const fs::path& file_path) { return fs::exists(file_path); }

		// The initialize_cache method is to be called at the start of every build.
		// It needs to check if a cache file already exists, and if not, then take the
		// include directories/cpp_files, and build the cache for the first time with them.
		// If the function returns true, then the cache exists and we can load it.
		// If it returns false, the cache was just created and all the cpp files need to be compiled.
		// TODO: A cache needs to be built for every package, so this needs to be called at the top of every
		// create_library/create_executable.
		// Return: true if cache was initialized, false is a cache already exists
		std::pair<bool, cache_storage> initialize_cache(const fs::path& cache_file_path,
		                                                const path_list& include_directories,
		                                                const path_list& cpp_files) {
			if(cache_exists(cache_file_path) == false) {
				_STD_BUILD_OUTPUT("file not found, creating new cache...");
				auto cache = build(include_directories, cpp_files);
				cache.write_to_file(cache_file_path);
				_STD_BUILD_OUTPUT("complete.\n");
				return { true, cache };
			}
			return { false, {} };
		}
	} // namespace __cache
} // namespace _STD_BUILD