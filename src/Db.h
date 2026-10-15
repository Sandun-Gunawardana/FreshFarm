#pragma once
#using <System.dll>
#using <System.Data.dll>
#using <System.Security.dll>
#using <MySql.Data.dll>
#include "Models.h"

using namespace System;
using namespace System::IO;
using namespace System::Data;
using namespace System::Security::Cryptography;
using namespace MySql::Data::MySqlClient;

namespace FreshFarm {
    public ref class Db abstract sealed {
    private:
        static String^ ReadSetting(String^ key, String^ fallback) {
            String^ path = Path::Combine(AppDomain::CurrentDomain->BaseDirectory, "dbconfig.ini");
            if (!File::Exists(path)) return fallback;
            for each (String^ line in File::ReadAllLines(path)) {
                String^ trimmed = line->Trim();
                if (trimmed->Length == 0 || trimmed->StartsWith("#") || trimmed->StartsWith(";")) continue;
                int pos = trimmed->IndexOf('=');
                if (pos <= 0) continue;
                String^ name = trimmed->Substring(0, pos)->Trim();
                if (String::Equals(name, key, StringComparison::OrdinalIgnoreCase))
                    return trimmed->Substring(pos + 1)->Trim();
            }
            return fallback;
        }

        static String^ ConnectionString() {
            String^ server = ReadSetting("Server", "127.0.0.1");
            String^ port = ReadSetting("Port", "3306");
            String^ database = ReadSetting("Database", "fresh_farm_db");
            String^ user = ReadSetting("User", "root");
            String^ password = ReadSetting("Password", "");
            MySqlConnectionStringBuilder^ b = gcnew MySqlConnectionStringBuilder();
            b->Server = server;
            b->Port = Convert::ToUInt32(port);
            b->Database = database;
            b->UserID = user;
            b->Password = password;
            b->Pooling = true;
            b->ConnectionTimeout = 8;
            b->AllowUserVariables = true;
            return b->ConnectionString;
        }

        static void AddParameters(MySqlCommand^ cmd, array<Object^>^ values) {
            if (values == nullptr) return;
            for (int i = 0; i < values->Length; ++i) {
                Object^ value = values[i];
                cmd->Parameters->AddWithValue("@p" + i.ToString(), value == nullptr ? DBNull::Value : value);
            }
        }

    public:
        static String^ HashPassword(String^ password) {
            array<Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(password);
            SHA256^ sha = SHA256::Create();
            array<Byte>^ hash = sha->ComputeHash(bytes);
            delete sha;
            return BitConverter::ToString(hash)->Replace("-", "")->ToLowerInvariant();
        }

        static void TestConnection() {
            MySqlConnection^ c = gcnew MySqlConnection(ConnectionString());
            try { c->Open(); }
            finally { c->Close(); delete c; }
        }

        static DataTable^ Query(String^ sql, array<Object^>^ values) {
            MySqlConnection^ c = gcnew MySqlConnection(ConnectionString());
            MySqlCommand^ cmd = gcnew MySqlCommand(sql, c);
            AddParameters(cmd, values);
            DataTable^ table = gcnew DataTable();
            try {
                c->Open();
                MySqlDataAdapter^ adapter = gcnew MySqlDataAdapter(cmd);
                adapter->Fill(table);
                delete adapter;
            }
            finally { delete cmd; c->Close(); delete c; }
            return table;
        }

        static int Execute(String^ sql, array<Object^>^ values) {
            MySqlConnection^ c = gcnew MySqlConnection(ConnectionString());
            MySqlCommand^ cmd = gcnew MySqlCommand(sql, c);
            AddParameters(cmd, values);
            try { c->Open(); return cmd->ExecuteNonQuery(); }
            finally { delete cmd; c->Close(); delete c; }
        }

        static Object^ Scalar(String^ sql, array<Object^>^ values) {
            MySqlConnection^ c = gcnew MySqlConnection(ConnectionString());
            MySqlCommand^ cmd = gcnew MySqlCommand(sql, c);
            AddParameters(cmd, values);
            try { c->Open(); return cmd->ExecuteScalar(); }
            finally { delete cmd; c->Close(); delete c; }
        }

        static Decimal RecordSale(int plantId, int quantity, String^ paymentMethod, int userId) {
            if (quantity < 1) throw gcnew ArgumentException("Quantity must be at least 1.");
            MySqlConnection^ c = gcnew MySqlConnection(ConnectionString());
            MySqlTransaction^ tx = nullptr;
            try {
                c->Open();
                tx = c->BeginTransaction();
                MySqlCommand^ select = gcnew MySqlCommand("SELECT PlantName, CategoryId, Price, StockQuantity, Description FROM plants WHERE PlantId=@p0 FOR UPDATE", c, tx);
                AddParameters(select, gcnew array<Object^>{ plantId });
                MySqlDataReader^ reader = select->ExecuteReader();
                if (!reader->Read()) {
                    reader->Close(); delete select;
                    throw gcnew InvalidOperationException("The selected plant no longer exists.");
                }
                Decimal unitPrice = Convert::ToDecimal(reader["Price"]);
                int stock = Convert::ToInt32(reader["StockQuantity"]);
                String^ plantName = Convert::ToString(reader["PlantName"]);
                int categoryId = Convert::ToInt32(reader["CategoryId"]);
                String^ description = reader["Description"] == DBNull::Value ? "" : Convert::ToString(reader["Description"]);
                reader->Close(); delete select;
                Plant^ plant = gcnew Plant(plantId, plantName, categoryId, unitPrice, stock, description);
                if (!plant->CanSell(quantity)) throw gcnew InvalidOperationException("Not enough stock. Available quantity: " + stock.ToString() + ".");
                Product^ product = plant; // Base-class reference demonstrates polymorphic product behaviour.
                Sale^ sale = gcnew Sale(product, quantity, paymentMethod, userId);
                Decimal total = sale->TotalAmount;

                MySqlCommand^ insert = gcnew MySqlCommand(
                    "INSERT INTO sales (PlantId, Quantity, UnitPrice, TotalAmount, PaymentMethod, SoldBy) VALUES (@p0,@p1,@p2,@p3,@p4,@p5)", c, tx);
                AddParameters(insert, gcnew array<Object^>{ plantId, quantity, unitPrice, sale->TotalAmount, sale->PaymentMethod, sale->SoldBy });
                insert->ExecuteNonQuery(); delete insert;

                MySqlCommand^ update = gcnew MySqlCommand("UPDATE plants SET StockQuantity=StockQuantity-@p0 WHERE PlantId=@p1", c, tx);
                AddParameters(update, gcnew array<Object^>{ quantity, plantId });
                update->ExecuteNonQuery(); delete update;
                tx->Commit();
                return total;
            }
            catch (Exception^) {
                // Attempt to rollback if transaction started
                if (tx != nullptr) {
                    try { tx->Rollback(); }
                    catch (...) { }
                }
                throw; // rethrow original exception
            }
            finally { if (c != nullptr) { c->Close(); delete c; } }
        }
    };
}
