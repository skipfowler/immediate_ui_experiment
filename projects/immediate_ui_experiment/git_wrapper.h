#pragma once

#include <git2/commit.h>
#include <git2/repository.h>

#include <functional>
#include <string>
#include <vector>

namespace Wrapper {
typedef std::function<void(const int, const char*)> GitErrorCallback;

class Git {
 public:
  Git();
  ~Git();

  auto BranchNames(git_repository* repo) -> std::vector<std::string_view>;
  auto Commit(git_repository* repository, git_oid oid) -> git_commit*;
  auto Commits(git_repository* repository) -> std::vector<git_commit*>;
  void ErrorCallback(GitErrorCallback error_callback);
  void Free(git_buf buffer);
  void Free(git_commit* commit);
  void Free(git_repository* repository);
  auto Head(git_repository* repository) -> std::string;
  auto LastCommit(git_repository* repository) -> git_commit*;
  auto NameToId(git_repository* repository, const char* name) -> git_oid;

  auto Repository(const char* start_path, const bool across_filesystem = false,
                  const char* ceiling_directories = nullptr) -> git_repository*;

  auto Root(const char* start_path, const bool across_filesystem = false,
            const char* ceiling_directories = nullptr) -> std::string;

 private:
  void HandleError(const int error_code);

  std::function<void(const int, const char*)> _error_callback = nullptr;
};
}  // namespace Wrapper