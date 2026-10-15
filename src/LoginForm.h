#pragma once
#include "Db.h"
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Data;

namespace FreshFarm {
    public ref class LoginForm : public Form {
    private:
        TextBox^ usernameBox;
        TextBox^ passwordBox;
        Label^ statusLabel;
        Button^ signInButton;

        Label^ MakeLabel(String^ text, int x, int y, int w, int h, float size, Color color, bool bold) {
            Label^ l = gcnew Label();
            l->Text = text; l->Location = Point(x, y); l->Size = Drawing::Size(w, h);
            l->Font = gcnew Drawing::Font("Segoe UI", size, bold ? FontStyle::Bold : FontStyle::Regular);
            l->ForeColor = color; l->BackColor = Color::Transparent;
            return l;
        }
        TextBox^ MakeInput(int x, int y, bool secret) {
            TextBox^ t = gcnew TextBox();
            t->Location = Point(x, y); t->Width = 300; t->Height = 38;
            t->Font = gcnew Drawing::Font("Segoe UI", 11);
            t->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
            if (secret) t->UseSystemPasswordChar = true;
            return t;
        }
        void OnSignIn(Object^, EventArgs^) {
            String^ username = usernameBox->Text->Trim();
            String^ password = passwordBox->Text;
            if (username->Length == 0 || password->Length == 0) {
                statusLabel->Text = "Enter your username and password.";
                statusLabel->ForeColor = Color::FromArgb(190, 52, 52);
                return;
            }
            signInButton->Enabled = false;
            statusLabel->Text = "Signing in...";
            statusLabel->ForeColor = Color::FromArgb(88, 101, 120);
            try {
                DataTable^ t = Db::Query(
                    "SELECT UserId, Username, FullName, Role FROM users WHERE Username=@p0 AND PasswordHash=@p1 AND IsActive=1 LIMIT 1",
                    gcnew array<Object^>{ username, Db::HashPassword(password) });
                if (t->Rows->Count != 1) {
                    statusLabel->Text = "Incorrect username or password.";
                    statusLabel->ForeColor = Color::FromArgb(190, 52, 52);
                    signInButton->Enabled = true;
                    return;
                }
                DataRow^ r = t->Rows[0];
                UserId = Convert::ToInt32(r["UserId"]);
                LoggedInUsername = Convert::ToString(r["Username"]);
                FullName = Convert::ToString(r["FullName"]);
                Role = Convert::ToString(r["Role"]);
                this->DialogResult = System::Windows::Forms::DialogResult::OK;
                this->Close();
            }
            catch (Exception^ ex) {
                statusLabel->Text = "Database connection failed. Check dbconfig.ini and run the SQL setup script.";
                statusLabel->ForeColor = Color::FromArgb(190, 52, 52);
                MessageBox::Show("Fresh Farm could not connect to MySQL.\n\n" + ex->Message +
                    "\n\nCheck that MySQL Server is running, the database exists, and dbconfig.ini contains the correct port and credentials.",
                    "Connection problem", MessageBoxButtons::OK, MessageBoxIcon::Error);
                signInButton->Enabled = true;
            }
        }
        void OnPasswordKey(Object^, KeyEventArgs^ e) { if (e->KeyCode == Keys::Enter) OnSignIn(nullptr, EventArgs::Empty); }

    public:
        int UserId;
        String^ LoggedInUsername;
        String^ FullName;
        String^ Role;

        LoginForm() {
            UserId = 0; LoggedInUsername = ""; FullName = ""; Role = "Staff";
            this->Text = "Fresh Farm | Sign in";
            this->StartPosition = FormStartPosition::CenterScreen;
            this->ClientSize = Drawing::Size(980, 620);
            this->MinimumSize = Drawing::Size(900, 580);
            this->BackColor = Color::FromArgb(245, 248, 246);
            this->Font = gcnew Drawing::Font("Segoe UI", 10);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximizeBox = false;

            Panel^ hero = gcnew Panel();
            hero->Dock = DockStyle::Left; hero->Width = 410;
            hero->BackColor = Color::FromArgb(25, 78, 58);
            this->Controls->Add(hero);
            hero->Controls->Add(MakeLabel("FRESH\nFARM", 44, 105, 320, 120, 32, Color::White, true));
            hero->Controls->Add(MakeLabel("Plant Sales Management", 46, 245, 315, 35, 14, Color::FromArgb(216, 237, 224), false));
            hero->Controls->Add(MakeLabel("A calmer way to manage\nyour plants, stock and sales.", 46, 305, 315, 72, 17, Color::White, false));
            hero->Controls->Add(MakeLabel("INVENTORY  ·  SALES  ·  REPORTING", 46, 540, 330, 25, 9, Color::FromArgb(190, 222, 202), true));

            Panel^ card = gcnew Panel();
            card->Location = Point(485, 78); card->Size = Drawing::Size(420, 455);
            card->BackColor = Color::White;
            card->Padding = System::Windows::Forms::Padding(36);
            this->Controls->Add(card);
            card->Controls->Add(MakeLabel("Welcome back", 36, 32, 340, 42, 23, Color::FromArgb(31, 45, 40), true));
            card->Controls->Add(MakeLabel("Sign in to continue to your workspace.", 36, 77, 340, 28, 10, Color::FromArgb(105, 119, 112), false));
            card->Controls->Add(MakeLabel("USERNAME", 36, 135, 300, 22, 9, Color::FromArgb(76, 91, 83), true));
            usernameBox = MakeInput(36, 160, false); card->Controls->Add(usernameBox);
            card->Controls->Add(MakeLabel("PASSWORD", 36, 215, 300, 22, 9, Color::FromArgb(76, 91, 83), true));
            passwordBox = MakeInput(36, 240, true); card->Controls->Add(passwordBox);
            signInButton = gcnew Button();
            signInButton->Text = "Sign in"; signInButton->Location = Point(36, 305); signInButton->Size = Drawing::Size(300, 43);
            signInButton->BackColor = Color::FromArgb(31, 107, 76); signInButton->ForeColor = Color::White;
            signInButton->FlatStyle = FlatStyle::Flat; signInButton->FlatAppearance->BorderSize = 0;
            signInButton->Font = gcnew Drawing::Font("Segoe UI Semibold", 11); signInButton->Cursor = Cursors::Hand;
            signInButton->Click += gcnew System::EventHandler(this, &LoginForm::OnSignIn); card->Controls->Add(signInButton);
            statusLabel = MakeLabel("Default account: admin  /  Admin123!", 36, 365, 325, 40, 9, Color::FromArgb(105, 119, 112), false);
            card->Controls->Add(statusLabel);
            passwordBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &LoginForm::OnPasswordKey);
            this->AcceptButton = signInButton;
        }
    };
}
