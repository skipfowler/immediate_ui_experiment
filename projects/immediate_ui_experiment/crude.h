#pragma once

// Conanfile fRom bUD gEnerator (crude)

#include <rapidjson/document.h>

#include <filesystem>
#include <string_view>
#include <vector>

namespace ABCDE {
class Crude {
 public:
  void AddGenerator(const char* generator);
  void AddImport(const char* import_in);
  void AddOption(const char* option);
  void AddRequirement(const char* requirement);

  void AddGenerators(const std::vector<std::string>& generators);
  void AddImports(const std::vector<std::string>& imports);
  void AddOptions(const std::vector<std::string>& options);
  void AddRequirements(const std::vector<std::string>& requirements);

  void AddGenerators(
      const rapidjson::GenericArray<false, rapidjson::Value>& generators);
  void AddImports(
      const rapidjson::GenericArray<false, rapidjson::Value>& imports);
  void AddOptions(
      const rapidjson::GenericArray<false, rapidjson::Value>& options);
  void AddRequirements(
      const rapidjson::GenericArray<false, rapidjson::Value>& requirements);

  void Clear();
  void ClearGenerators();
  void ClearImports();
  void ClearOptions();
  void ClearRequirements();

  auto GetGenerators() const -> const std::vector<std::string>&;
  auto GetImports() const -> const std::vector<std::string>&;
  auto GetOptions() const -> const std::vector<std::string>&;
  auto GetRequirements() const -> const std::vector<std::string>&;

  auto ReadConanfile(std::string_view input_file) -> bool;
  //auto ReadJson(std::string_view input_file) -> bool;

  auto RemoveGenerator(std::string_view generator) -> bool;
  auto RemoveGenerators(const std::vector<std::string_view>& generators)
      -> bool;
  auto RemoveImport(std::string_view import_in) -> bool;
  auto RemoveImports(const std::vector<std::string_view>& imports)
      -> bool;
  auto RemoveOption(std::string_view option) -> bool;
  auto RemoveOptions(const std::vector<std::string_view>& options)
      -> bool;
  auto RemoveRequirement(std::string_view requirement) -> bool;
  auto RemoveRequirements(const std::vector<std::string_view>& requirements) -> bool;

  auto ToJson(rapidjson::Document& document) -> const rapidjson::Value&;

  auto WriteConanfile(std::string_view output_directory) const -> bool;

 private:
  std::vector<std::string> generators_;
  std::vector<std::string> imports_;
  std::vector<std::string> options_;
  std::vector<std::string> requirements_;
};

auto CreateDirectory(const std::filesystem::path& directory) -> bool;
}  // namespace ABCDE