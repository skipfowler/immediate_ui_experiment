#include "azure_devops_wrapper.h"
#pragma managed

#include <msclr\auto_gcroot.h>

#include "azure_devops_calls.cpp"

using namespace System::Runtime::InteropServices;

class AzureDevOpsCallsPrivate {
 public:
  msclr::auto_gcroot<AzureDevOpsCalls ^>
      adoInst;  // For Managed-to-Unmanaged marshalling
};

AzureDevOpsWrapper::AzureDevOpsWrapper() {
  helper = new AzureDevOpsCallsPrivate();
  helper->adoInst = gcnew AzureDevOpsCalls();
}

AzureDevOpsWrapper::~AzureDevOpsWrapper() { delete helper; }

void AzureDevOpsWrapper::ExecuteCommand() { helper->adoInst->SayHi(); }