#pragma once
using namespace System;

namespace FreshFarm {
    // Abstract base type shared by products sold by Fresh Farm.
    public ref class Product abstract {
    private:
        int productId;
        String^ productName;
        Decimal unitPrice;
    public:
        Product(int id, String^ name, Decimal price) {
            productId = id; productName = name; unitPrice = price;
        }
        property int ProductId { int get() { return productId; } }
        property String^ ProductName { String^ get() { return productName; } }
        property Decimal UnitPrice { Decimal get() { return unitPrice; } }
        virtual String^ GetSummary() abstract;
        virtual Decimal CalculateTotal(int quantity) {
            if (quantity < 1) throw gcnew ArgumentOutOfRangeException("quantity", "Quantity must be at least one.");
            return unitPrice * Convert::ToDecimal(quantity);
        }
    };

    // Plant inherits the common product data and adds inventory-specific information.
    public ref class Plant : public Product {
    private:
        int categoryId;
        int stockQuantity;
        String^ description;
    public:
        Plant(int id, String^ name, int category, Decimal price, int stock, String^ details)
            : Product(id, name, price) {
            categoryId = category; stockQuantity = stock; description = details;
        }
        property int CategoryId { int get() { return categoryId; } }
        property int StockQuantity { int get() { return stockQuantity; } }
        property String^ Description { String^ get() { return description; } }
        bool CanSell(int quantity) { return quantity > 0 && quantity <= stockQuantity; }
        virtual Decimal CalculateTotal(int quantity) override {
            if (!CanSell(quantity)) throw gcnew InvalidOperationException("Requested quantity exceeds available stock.");
            return Product::CalculateTotal(quantity);
        }
        virtual String^ GetSummary() override {
            return ProductName + " — Rs. " + UnitPrice.ToString("N2") + " (stock: " + stockQuantity.ToString() + ")";
        }
    };

    public ref class Category sealed {
    private:
        int categoryId;
        String^ categoryName;
        String^ description;
    public:
        Category(int id, String^ name, String^ details) { categoryId = id; categoryName = name; description = details; }
        property int CategoryId { int get() { return categoryId; } }
        property String^ CategoryName { String^ get() { return categoryName; } }
        property String^ Description { String^ get() { return description; } }
    };

    // A Sale calculates its amount from the Product abstraction; it doesn't trust a UI-supplied total.
    public ref class Sale sealed {
    private:
        int quantity;
        Decimal totalAmount;
        String^ paymentMethod;
        int soldBy;
    public:
        Sale(Product^ product, int qty, String^ method, int userId) {
            if (product == nullptr) throw gcnew ArgumentNullException("product");
            if (qty < 1) throw gcnew ArgumentOutOfRangeException("qty");
            quantity = qty; paymentMethod = method; soldBy = userId;
            totalAmount = product->CalculateTotal(qty);
        }
        property int Quantity { int get() { return quantity; } }
        property Decimal TotalAmount { Decimal get() { return totalAmount; } }
        property String^ PaymentMethod { String^ get() { return paymentMethod; } }
        property int SoldBy { int get() { return soldBy; } }
    };
}
