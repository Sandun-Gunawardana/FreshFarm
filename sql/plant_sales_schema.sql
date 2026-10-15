-- Fresh Farm Plant Sales Management System
-- MySQL 8.x. Run this script in MySQL Workbench or with the mysql command-line client.
CREATE DATABASE IF NOT EXISTS fresh_farm_db
  CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE fresh_farm_db;

CREATE TABLE IF NOT EXISTS users (
    UserId INT NOT NULL AUTO_INCREMENT,
    Username VARCHAR(60) NOT NULL,
    FullName VARCHAR(120) NOT NULL,
    PasswordHash CHAR(64) NOT NULL,
    Role ENUM('Admin','Staff') NOT NULL DEFAULT 'Staff',
    IsActive TINYINT(1) NOT NULL DEFAULT 1,
    CreatedAt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (UserId),
    UNIQUE KEY uq_users_username (Username)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS categories (
    CategoryId INT NOT NULL AUTO_INCREMENT,
    CategoryName VARCHAR(100) NOT NULL,
    Description VARCHAR(500) NULL,
    CreatedAt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (CategoryId),
    UNIQUE KEY uq_categories_name (CategoryName)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS plants (
    PlantId INT NOT NULL AUTO_INCREMENT,
    PlantName VARCHAR(150) NOT NULL,
    CategoryId INT NOT NULL,
    Price DECIMAL(10,2) NOT NULL,
    StockQuantity INT NOT NULL DEFAULT 0,
    Description VARCHAR(500) NULL,
    CreatedAt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UpdatedAt TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    PRIMARY KEY (PlantId),
    KEY ix_plants_category (CategoryId),
    KEY ix_plants_name (PlantName),
    CONSTRAINT fk_plants_category FOREIGN KEY (CategoryId)
      REFERENCES categories(CategoryId) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT chk_plants_price CHECK (Price > 0),
    CONSTRAINT chk_plants_stock CHECK (StockQuantity >= 0)
) ENGINE=InnoDB;

CREATE TABLE IF NOT EXISTS sales (
    SaleId BIGINT NOT NULL AUTO_INCREMENT,
    PlantId INT NOT NULL,
    Quantity INT NOT NULL,
    UnitPrice DECIMAL(10,2) NOT NULL,
    TotalAmount DECIMAL(12,2) NOT NULL,
    PaymentMethod VARCHAR(40) NOT NULL,
    SoldBy INT NOT NULL,
    SaleDate TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (SaleId),
    KEY ix_sales_date (SaleDate),
    KEY ix_sales_plant (PlantId),
    CONSTRAINT fk_sales_plant FOREIGN KEY (PlantId)
      REFERENCES plants(PlantId) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT fk_sales_user FOREIGN KEY (SoldBy)
      REFERENCES users(UserId) ON UPDATE CASCADE ON DELETE RESTRICT,
    CONSTRAINT chk_sales_quantity CHECK (Quantity > 0)
) ENGINE=InnoDB;

-- Demo account: username admin, password Admin123!
-- The application hashes passwords with SHA-256 before comparing them.
INSERT INTO users (Username, FullName, PasswordHash, Role, IsActive)
VALUES ('admin', 'Fresh Farm Administrator', SHA2('Admin123!', 256), 'Admin', 1)
ON DUPLICATE KEY UPDATE Username = VALUES(Username);

INSERT IGNORE INTO categories (CategoryName, Description) VALUES
('Flowering Plants', 'Plants grown for decorative flowers.'),
('Indoor Plants', 'Plants suited to indoor spaces.'),
('Succulents', 'Low-maintenance plants that store water.'),
('Herbs', 'Useful culinary and aromatic plants.'),
('Outdoor Plants', 'Plants suited to gardens and outdoor areas.');

INSERT INTO plants (PlantName, CategoryId, Price, StockQuantity, Description)
SELECT 'Peace Lily', CategoryId, 1850.00, 12, 'Indoor plant with elegant white blooms.' FROM categories WHERE CategoryName='Indoor Plants'
AND NOT EXISTS (SELECT 1 FROM plants WHERE PlantName='Peace Lily');
INSERT INTO plants (PlantName, CategoryId, Price, StockQuantity, Description)
SELECT 'Mini Rose', CategoryId, 1250.00, 18, 'Compact rose plant with colourful flowers.' FROM categories WHERE CategoryName='Flowering Plants'
AND NOT EXISTS (SELECT 1 FROM plants WHERE PlantName='Mini Rose');
INSERT INTO plants (PlantName, CategoryId, Price, StockQuantity, Description)
SELECT 'Aloe Vera', CategoryId, 950.00, 4, 'Hardy succulent for a sunny spot.' FROM categories WHERE CategoryName='Succulents'
AND NOT EXISTS (SELECT 1 FROM plants WHERE PlantName='Aloe Vera');
INSERT INTO plants (PlantName, CategoryId, Price, StockQuantity, Description)
SELECT 'Basil', CategoryId, 450.00, 25, 'Fresh culinary herb.' FROM categories WHERE CategoryName='Herbs'
AND NOT EXISTS (SELECT 1 FROM plants WHERE PlantName='Basil');
INSERT INTO plants (PlantName, CategoryId, Price, StockQuantity, Description)
SELECT 'Jade Plant', CategoryId, 1100.00, 7, 'Decorative, easy-care succulent.' FROM categories WHERE CategoryName='Succulents'
AND NOT EXISTS (SELECT 1 FROM plants WHERE PlantName='Jade Plant');
