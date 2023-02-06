#include "crude.h"

#include <spdlog/spdlog.h>

#include <fstream>

namespace ABCDE {
void Crude::AddGenerator(const char* generator) {
  generators_.push_back(generator);
}

void Crude::AddImport(const char* import_in) { imports_.push_back(import_in); }

void Crude::AddOption(const char* option) { options_.push_back(option); }

void Crude::AddRequirement(const char* requirement) {
  requirements_.push_back(requirement);
}

void Crude::AddGenerators(const std::vector<std::string>& generators) {
  for (auto& generator : generators) {
    generators_.push_back(generator);
  }
}

void Crude::AddImports(const std::vector<std::string>& imports) {
  for (auto& import_in : imports) {
    imports_.push_back(import_in);
  }
}
void Crude::AddOptions(const std::vector<std::string>& options) {
  for (auto& option : options) {
    options_.push_back(option);
  }
}

void Crude::AddRequirements(const std::vector<std::string>& requirements) {
  for (auto& requirement : requirements) {
    requirements_.push_back(requirement);
  }
}

void Crude::AddGenerators(
    const rapidjson::GenericArray<false, rapidjson::Value>& generators) {
  for (auto& generator : generators) {
    generators_.push_back(generator.GetString());
  }
}

void Crude::AddImports(
    const rapidjson::GenericArray<false, rapidjson::Value>& imports) {
  for (auto& import_in : imports) {
    imports_.push_back(import_in.GetString());
  }
}

void Crude::AddOptions(
    const rapidjson::GenericArray<false, rapidjson::Value>& options) {
  for (auto& option : options) {
    options_.push_back(option.GetString());
  }
}

void Crude::AddRequirements(
    const rapidjson::GenericArray<false, rapidjson::Value>& requirements) {
  for (auto& requirement : requirements) {
    requirements_.push_back(requirement.GetString());
  }
}

void Crude::Clear() {
  ClearGenerators();
  ClearImports();
  ClearOptions();
  ClearRequirements();
}

void Crude::ClearGenerators() { generators_.clear(); }

void Crude::ClearImports() { imports_.clear(); }

void Crude::ClearOptions() { options_.clear(); }

void Crude::ClearRequirements() { requirements_.clear(); }

auto Crude::GetGenerators() const -> const std::vector<std::string>& {
  return generators_;
}

auto Crude::GetImports() const -> const std::vector<std::string>& {
  return imports_;
}

auto Crude::GetOptions() const -> const std::vector<std::string>& {
  return options_;
}

auto Crude::GetRequirements() const -> const std::vector<std::string>& {
  return requirements_;
}

auto Crude::ReadConanfile(std::string_view input_file) -> bool {
  // Denote current section of Conanfile.txt
  enum class ConanfileReadType {
    kGenerators,
    kImports,
    kNone,
    kOptions,
    kRequires
  };

  // Check file makes sense
  std::filesystem::path path(input_file);

  if (!std::filesystem::is_regular_file(path)) {
    spdlog::error("Unable to read {} since it is not a file", input_file);
    spdlog::dump_backtrace();
    return false;
  }

  // Read lines from file and assign to correct collection
  ConanfileReadType read_type = ConanfileReadType::kNone;
  std::ifstream ifs(input_file.data());
  std::string line;

  while (std::getline(ifs, line)) {
    if ('[' == line[0]) {
      if (0 == line.compare("[requires]")) {
        read_type = ConanfileReadType::kRequires;
      } else if (0 == line.compare("[generators]")) {
        read_type = ConanfileReadType::kGenerators;
      } else if (0 == line.compare("[options]")) {
        read_type = ConanfileReadType::kOptions;
      } else if (0 == line.compare("[imports]")) {
        read_type = ConanfileReadType::kImports;
      }

      continue;
    }

    switch (read_type) {
      case ConanfileReadType::kGenerators:
        generators_.push_back(line);
        break;
      case ConanfileReadType::kImports:
        imports_.push_back(line);
        break;
      case ConanfileReadType::kOptions:
        options_.push_back(line);
        break;
      case ConanfileReadType::kRequires:
        requirements_.push_back(line);
        break;
      default:
        break;
    }
  }

  return true;
}

auto Crude::RemoveGenerator(std::string_view generator) -> bool {
  auto it = std::ranges::find(generators_, generator);

  if (it != generators_.end()) {
    generators_.erase(it);
    return true;
  }

  return false;
}

auto Crude::RemoveGenerators(const std::vector<std::string_view>& generators)
    -> bool {
  bool all_found = true;

  for (auto& generator : generators) {
    all_found &= RemoveGenerator(generator);
  }

  return all_found;
}

auto Crude::RemoveImport(std::string_view import_in) -> bool {
  auto it = std::ranges::find(imports_, import_in);

  if (it != imports_.end()) {
    imports_.erase(it);
    return true;
  }

  return false;
}

auto Crude::RemoveImports(const std::vector<std::string_view>& imports)
    -> bool {
  bool all_found = true;

  for (auto& import_in : imports) {
    all_found &= RemoveImport(import_in);
  }

  return all_found;
}

auto Crude::RemoveOption(std::string_view option) -> bool {
  auto it = std::ranges::find(options_, option);

  if (it != options_.end()) {
    options_.erase(it);
    return true;
  }

  return false;
}

auto Crude::RemoveOptions(const std::vector<std::string_view>& options)
    -> bool {
  bool all_found = true;

  for (auto& option : options) {
    all_found &= RemoveOption(option);
  }

  return all_found;
}

auto Crude::RemoveRequirement(std::string_view requirement) -> bool {
  auto it = std::ranges::find(requirements_, requirement);

  if (it != requirements_.end()) {
    requirements_.erase(it);
    return true;
  }

  return false;
}

auto Crude::RemoveRequirements(
    const std::vector<std::string_view>& requirements) -> bool {
  bool all_found = true;

  for (auto& requirement : requirements) {
    all_found &= RemoveRequirement(requirement);
  }

  return all_found;
}

auto Crude::ToJson(rapidjson::Document& document) -> const rapidjson::Value& {
  rapidjson::Value root_node("conan");

  {
    auto& node = root_node.AddMember("generators", "", document.GetAllocator());
  }

  { auto& node = root_node.AddMember("imports", "", document.GetAllocator()); }

  { auto& node = root_node.AddMember("options", "", document.GetAllocator()); }

  { auto& node = root_node.AddMember("requires", "", document.GetAllocator()); }

  return root_node;
}

auto Crude::WriteConanfile(std::string_view output_directory) const -> bool {
  // Create file stream
  if (!CreateDirectory(output_directory)) {
    spdlog::error("Unable to write Conanfile.txt to {}", output_directory);
    return false;
  }

  std::filesystem::path output_file(output_directory);
  output_file.append("conanfile.txt");

  std::ofstream ofs(output_file.c_str());

  // Write Conanfile.txt
  ofs << "[requires]" << std::endl;

  for (auto& requirement : requirements_) {
    ofs << requirement << std::endl;
  }

  ofs << "[generators]" << std::endl;

  for (auto& generator : generators_) {
    ofs << generator << std::endl;
  }

  ofs << "[options]" << std::endl;

  for (auto& option : options_) {
    ofs << option << std::endl;
  }

  ofs << "[imports]" << std::endl;

  for (auto& import_in : imports_) {
    ofs << import_in << std::endl;
  }

  return true;
}

auto CreateDirectory(const std::filesystem::path& directory) -> bool {
  std::filesystem::path path(directory);

  if (std::filesystem::is_regular_file(path)) {
    spdlog::error("{} is not a valid directory");
    spdlog::dump_backtrace();
    return false;
  }

  if (!std::filesystem::is_directory(path)) {
    std::filesystem::create_directories(path);
  }

  return true;
}
}  // namespace ABCDE