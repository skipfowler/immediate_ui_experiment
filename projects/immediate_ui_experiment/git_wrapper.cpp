#include "git_wrapper.h"

#include <spdlog/spdlog.h>

#include "git2.h"

namespace Wrapper {
Git::Git() { git_libgit2_init(); }

Git::~Git() { git_libgit2_shutdown(); }

auto Git::BranchNames(git_repository* repository)
    -> std::vector<std::string_view> {
  std::vector<std::string_view> branch_names{};
  git_branch_iterator* it;

  if (!git_branch_iterator_new(&it, repository, GIT_BRANCH_ALL)) {
    git_reference* ref;
    git_branch_t type;

    while (!git_branch_next(&ref, &type, it)) {
      branch_names.push_back(git_reference_name(ref));
      git_reference_free(ref);
    }

    git_branch_iterator_free(it);
  }

  return branch_names;
}

auto Git::Commit(git_repository* repository, git_oid oid) -> git_commit* {
  git_commit* commit = nullptr;
  int error = git_commit_lookup(&commit, repository, &oid);

  if (GIT_OK != error) {
    HandleError(error);
  }

  return commit;
}

auto Git::Commits(git_repository* repository) -> std::vector<git_commit*> {
  return std::vector<git_commit*>();
}

void Git::ErrorCallback(GitErrorCallback error_callback) {
  _error_callback = error_callback;
}

void Git::Free(git_buf buffer) { git_buf_free(&buffer); }

void Git::Free(git_commit* commit) { git_commit_free(commit); }

void Git::Free(git_repository* repository) { git_repository_free(repository); }

void Git::HandleError(const int error_code) {
  spdlog::error("git error {}: {}", error_code, git_error_last()->message);

  if (nullptr != _error_callback) {
    _error_callback(error_code, git_error_last()->message);
  }
}

auto Git::Head(git_repository* repository) -> std::string {
  git_reference* ref = nullptr;
  int error = git_repository_head(&ref, repository);

  if (GIT_OK != error) {
    HandleError(error);
  }

  return git_reference_name(ref);
}

auto Git::LastCommit(git_repository* repository) -> git_commit* {
  std::string name = Head(repository);
  git_oid oid_parent_commit = NameToId(repository, name.c_str());
  return Commit(repository, oid_parent_commit);
}

auto Git::NameToId(git_repository* repository, const char* name) -> git_oid {
  git_oid oid;
  int error = git_reference_name_to_id(&oid, repository, name);

  if (GIT_OK != error) {
    HandleError(error);
  }

  return oid;
}

auto Git::Repository(const char* start_path, const bool across_filesystem,
                     const char* ceiling_directories) -> git_repository* {
  std::string root = Root(start_path, across_filesystem, ceiling_directories);
  git_repository* repository = nullptr;
  int error = git_repository_open(&repository, root.c_str());

  if (GIT_OK != error) {
    HandleError(error);
  }

  return repository;
}

auto Git::Root(const char* start_path, const bool across_filesystem,
               const char* ceiling_directories) -> std::string {
  std::string root;
  git_buf root_raw = {0};
  int error =
      git_repository_discover(&root_raw, start_path, 0, ceiling_directories);

  if (GIT_OK == error) {
    if (nullptr != root_raw.ptr) {
      root = root_raw.ptr;
    } else {
      spdlog::error("git root is nullptr");
    }
  } else {
    HandleError(error);
  }

  Free(root_raw);
  return root;
}
}  // namespace Wrapper