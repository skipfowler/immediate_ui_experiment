#pragma once
#pragma unmanaged

#include <iostream>

#if defined(WIN32)
#if defined(azure_devops_experiment_EXPORTS)
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#else
#define DLL
#endif

class AzureDevOpsCallsPrivate;

class DLL AzureDevOpsWrapper {
 public:
  AzureDevOpsWrapper();
  ~AzureDevOpsWrapper();

  void ExecuteCommand();
  void UnmangedCommand() { std::cout << "Hello from unmanaged!" << std::endl; }

 private:
  AzureDevOpsCallsPrivate* helper;
};