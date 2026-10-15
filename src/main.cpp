#include "LoginForm.h"
#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    while (true) {
        FreshFarm::LoginForm^ login = gcnew FreshFarm::LoginForm();
        System::Windows::Forms::DialogResult loginResult = login->ShowDialog();
        if (loginResult != System::Windows::Forms::DialogResult::OK) break;

        FreshFarm::MainForm^ mainWindow = gcnew FreshFarm::MainForm(
            login->UserId, login->LoggedInUsername, login->FullName, login->Role);
        Application::Run(mainWindow);
        if (mainWindow->DialogResult != System::Windows::Forms::DialogResult::Retry) break;
    }

    return 0;
}

// Provide WinMain for Windows subsystem entry (avoids linker error when subsystem:windows is used)
extern "C" int __stdcall WinMain(void*, void*, char*, int) {
    return main(gcnew array<String^>(0));
}
