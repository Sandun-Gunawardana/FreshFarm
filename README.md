# Fresh Farm — Plant Sales Management System

A modern desktop **Plant Sales Management System** built with **C++/CLI + Windows Forms (.NET Framework 4.7.2) + MySQL**. Features a responsive botanical-themed UI with complete CRUD operations, database transactions, and real-time inventory management.

## Features

✅ **Authentication**
- Secure login with SHA-256 password hashing
- User roles: Admin and Staff
- Session state management

✅ **Dashboard**
- Key metrics: Total Plants, Categories, Today's Revenue, Low-Stock Count
- Responsive stat cards that adapt to window width
- Quick-access low-stock items table
- Real-time database queries

✅ **Plant Management**
- Add/Edit/Delete plants with PlantEditorForm modal dialog
- Comprehensive search by name or description
- Category assignment (dropdown from database)
- Price and stock quantity management
- Bulk inventory display with category resolution via SQL JOIN

✅ **Category Management**
- Add/Edit/Delete categories with validation
- **Referential integrity protection**: Prevents deletion of categories referenced by plants
- Search and filter functionality

✅ **Sales Management**
- Complete sales workflow (form + history table - implementation in progress)
- ACID transaction support: sale creation + stock deduction in single transaction
- Automatic rollback on insufficient inventory
- Real-time transaction validation
- CSV export capability

✅ **Reports** (Implementation in progress)
- Sales analytics
- Inventory summary
- Best-selling plants
- CSV export for analysis

✅ **User Interface**
- Botanical green theme (#174F3B, #237653, #5DBE8F)
- Responsive layout adapts to window resize
- Modal dialogs for add/edit operations
- Professional DataGridView styling
- Consistent fonts and spacing (Segoe UI, 16px margins)
- Clear error messages and validation feedback

## Architecture

### Technology Stack
- **Language**: C++/CLI (managed C++)
- **UI Framework**: Windows Forms (.NET Framework 4.7.2)
- **Database**: MySQL 8.0+
- **Connector**: MySQL Connector/NET (MySql.Data.dll)
- **Authentication**: SHA-256 password hashing

### Key Components
1. **LoginForm.h** (170 lines) - User authentication UI
2. **MainForm.h** (~1365 lines) - Main application shell with all pages, CRUD operations, Sales workflow, and Reports analytics
3. **Db.h** (152 lines) - Database abstraction layer with Query(), Execute(), Scalar(), RecordSale(); transactional sales recording
4. **Models.h** (80 lines) - Domain models: Product, Plant, Category, Sale
5. **main.cpp** - Application entry point with STAThread attribute

### Database Schema
- **users** - Staff accounts with role-based access
- **categories** - Product categories with ON DELETE RESTRICT foreign key constraint
- **plants** - Inventory items with category assignment and pricing
- **sales** - Transaction records with transactional consistency

## Requirements

### Windows System
- Windows 10/11 (x86 or x64)
- Visual Studio Community 2022+ with:
  - **Desktop development with C++** workload
  - **.NET desktop development** workload
  - **C++/CLI support** for your MSVC toolset
  - **.NET Framework 4.7.2 SDK** and targeting pack (under Individual Components)

### Database
- **MySQL Server 8.0** or compatible newer version
- **MySQL Connector/NET** (provides MySql.Data.dll for .NET Framework)
  - Download from: https://dev.mysql.com/downloads/connector/net/
  - Common installation path: `C:\Program Files (x86)\MySQL\MySQL Connector NET <version>\Assemblies\v4.5.2\`

### Project Files
- FreshFarm.vcxproj - Visual Studio C++ project file
- dbconfig.ini - Runtime database configuration
- sql/plant_sales_schema.sql - Database initialization script

## Setup Instructions

### 1. Prepare Development Environment

Open Visual Studio Installer → **Modify**:
- Ensure **Desktop development with C++** is selected
- Ensure **.NET desktop development** is selected
- Under **Individual Components** (All → SDKs, libraries, and frameworks):
  - Check **C++/CLI support for v142 build tools** (or your MSVC version)
  - Check **.NET Framework 4.7.2 SDK**
  - Check **.NET Framework 4.7.2 targeting pack**
- Click **Modify** and wait for installation

### 2. Install MySQL Connector/NET

1. Download MySQL Connector/NET 8.0 (or latest compatible version) from:
   https://dev.mysql.com/downloads/connector/net/

2. Run the installer. Default installation path is usually:
   - x86: `C:\Program Files (x86)\MySQL\MySQL Connector NET 8.0\Assemblies\v4.5.2\MySql.Data.dll`
   - x64: `C:\Program Files\MySQL\MySQL Connector NET 8.0\Assemblies\v4.5.2\MySql.Data.dll`

3. **If MySql.Data.dll is not found after building:**
   - Locate the actual DLL path on your computer
   - Open `FreshFarm.vcxproj` (right-click project → Edit Project File)
   - Find the `<MySqlConnectorDll>` property
   - Update the path to match your installation, e.g.:
     ```xml
     <MySqlConnectorDll>C:\Program Files (x86)\MySQL\MySQL Connector NET 8.0\Assemblies\v4.5.2\MySql.Data.dll</MySqlConnectorDll>
     ```
   - Save, close the project, and reload it in Visual Studio

### 3. Set Up MySQL Database

1. Start MySQL Server:
   ```powershell
   # On Windows with MySQL service installed:
   net start MySQL80
   ```

2. Open MySQL Workbench or command-line:
   ```bash
   mysql -u root -p
   # Enter your root password
   ```

3. Run the SQL initialization script:
   ```bash
   source sql/plant_sales_schema.sql;
   # Or in Windows:
   source C:\path\to\FreshFarm_Project\sql\plant_sales_schema.sql;
   ```

4. Verify the database was created:
   ```sql
   USE fresh_farm_db;
   SELECT COUNT(*) FROM plants;
   ```

### 4. Configure Database Connection

Edit `dbconfig.ini` in the project root:
```ini
Server=127.0.0.1
Port=3306
Database=fresh_farm_db
User=root
Password=your_mysql_root_password
```

**Important**: Do **not** commit `dbconfig.ini` with real passwords. Add it to `.gitignore`.

### 5. Build and Run

1. Open `FreshFarm.vcxproj` in Visual Studio
2. Select **Build → Rebuild Solution** (or press Ctrl+Alt+F7)
3. Verify build completes without errors
4. Press **F5** or **Debug → Start Debugging** to launch

### 6. First Login

The SQL script creates a demo user:
- **Username**: `admin`
- **Password**: `Admin123!`

Use these credentials to log in.

## Deployment to Another Windows Computer

### Prerequisites on Target Computer
- Windows 10/11
- **.NET Framework 4.7.2 Runtime** (install if not present)
- **MySQL Server 8.0**+ and MySQL Connector/NET
- The `dbconfig.ini` with correct database connection details

### Steps
1. Copy the compiled executable (`FreshFarm.exe`) and `dbconfig.ini` to the target folder
2. Ensure `dbconfig.ini` points to the target computer's MySQL server
3. Copy the `sql/plant_sales_schema.sql` to the target
4. Run the SQL script to initialize the database on the target server
5. Run `FreshFarm.exe`

**Note**: The application expects `dbconfig.ini` to be in the same directory as `FreshFarm.exe`.

## Database Connection Troubleshooting

### "Could not connect to localhost"
- Verify MySQL Server is running: `net start MySQL80` (Windows)
- Check `dbconfig.ini` has correct Server, Port, User, Password
- Verify the `fresh_farm_db` database exists: `SHOW DATABASES;`

### "Access denied for user 'root'"
- Verify password in `dbconfig.ini` matches your MySQL root password
- Reset MySQL root password if forgotten

### "Unknown/unsupported authentication plugin 'caching_sha2_password'"
- MySQL 8.0 uses caching_sha2_password by default
- Ensure MySQL Connector/NET version is 8.0 or later (supports this plugin)
- Or change the user's authentication plugin:
  ```sql
  ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'your_password';
  FLUSH PRIVILEGES;
  ```

## Code Organization

### MainForm.h Structure (~1365 lines)
```
Theme Colors (8 colors: Forest Green, Action Green, Light Green, etc.)
  ↓
UI Layout (Sidebar + Header + Content Panel with responsive Resize handler)
  ↓
Navigation Methods (OnDashboardNav, OnPlantsNav, etc.)
  ↓
Dashboard (ShowDashboard, CreateStatCard, Total/Low-Stock cards reload on resize)
  ↓
Plants Page (ShowPlants, LoadPlants, OnAddPlant, OnEditPlant, OnDeletePlant, Search filter)
  ↓
PlantEditorForm (Modal dialog with form validation, category selector)
  ↓
Categories Page (ShowCategories, OnAddCategory, OnEditCategory, OnDeleteCategory with FK checks)
  ↓
Sales Page (ShowSales with plant selector, quantity, payment method; RecordSale button; Recent Sales grid)
  ↓
Reports Page (Sales Summary by payment, Inventory Status, Best-Selling Plants; CSV export buttons)
  ↓
Logout Handler
```

### Database Query Patterns
All user-provided values use parameterized queries:
```cpp
// Correct - prevents SQL injection
Db::Query("SELECT * FROM plants WHERE PlantName LIKE @p0", 
          gcnew array<Object^> { "%" + searchTerm + "%" });

// Wrong - vulnerable
Db::Query("SELECT * FROM plants WHERE PlantName LIKE '%" + searchTerm + "%'");
```

## Development Guide

### Adding a New CRUD Page
1. Create a `Show<Page>()` method in MainForm class
2. Implement `Load<Page>()` for database queries
3. Create editor dialogs or forms
4. Add navigation button handler: `void On<Page>Nav(Object^, EventArgs^)`
5. Add button to sidebar in `CreateSidebar()`
6. Call `PreparePage()` at start of `Show<Page>()` to clear controls and set title

### Adding Database Queries
- Use `Db::Query()` for SELECT (returns DataTable)
- Use `Db::Execute()` for INSERT/UPDATE/DELETE (returns affected row count)
- Use `Db::Scalar()` for COUNT/SUM/single values (returns Object^)
- Always use parameterized queries with @p0, @p1, @p2, etc.

### Styling Guidelines
- Use the 8 predefined theme colors (ColorForestGreen, ColorActionGreen, etc.)
- Button heights: 36-40px
- Padding: 16px external, 8px internal
- Font: Segoe UI throughout
- DataGridView header height: 36px, row height: 28px

## Testing Checklist

- [ ] Application launches and login screen displays
- [ ] Login with admin/Admin123! succeeds
- [ ] Dashboard displays actual statistics from database
- [ ] Cards resize smoothly when window is resized
- [ ] Low-stock items table shows correct data
- [ ] Plants page: Add plant creates database record
- [ ] Plants page: Edit plant modifies database record
- [ ] Plants page: Delete plant removes record (or error if stock exhausted)
- [ ] Plants page: Search filters results correctly
- [ ] Categories page: Add category works
- [ ] Categories page: Edit category name works
- [ ] Categories page: Cannot delete category if plants reference it
- [ ] Sales page: Select plant, quantity, and payment method
- [ ] Sales page: Click "Record Sale" reduces stock and shows success
- [ ] Sales page: Recent sales grid updates with new transaction
- [ ] Sales page: Can see all recent sales with dates and totals
- [ ] Reports page: Sales Summary tab shows transactions grouped by payment method
- [ ] Reports page: Inventory Status tab shows stock levels and inventory values
- [ ] Reports page: Best-Selling Plants tab shows units and revenue per plant
- [ ] Reports page: Export Sales to CSV creates downloadable file
- [ ] Reports page: Export Inventory to CSV creates downloadable file
- [ ] All pages navigate without leaving stale controls
- [ ] Grid data displays category names (via JOIN), not IDs
- [ ] Error messages display for validation and database failures
- [ ] Sign-out logs back to login screen

## Known Limitations

- No user account management UI (managed via database directly)
- No multi-language support
- No dark mode toggle
- Grid columns use auto-sizing (Fill) - custom widths would require per-page adjustment
- Reports page uses .NET Framework 4.7.2 compatible CSV export (basic text-only, no Unicode escaping enhancement)

## License & Credits

Built as a demonstration of C++/CLI Windows Forms development with MySQL for a plant sales business domain. Feel free to extend with your own features.

## Support & Troubleshooting

If the application crashes or fails to load:
1. Check that `dbconfig.ini` exists and has valid database credentials
2. Verify MySQL Server is running and contains the `fresh_farm_db` database
3. Check Visual Studio Output window for detailed error messages
4. Verify all required .NET Framework and C++ components are installed

For detailed error messages, check the application output in Visual Studio's Debug window (Debug → Windows → Output).In MySQL Workbench, connect to your local server and open/run:

`sql/plant_sales_schema.sql`

The script creates `fresh_farm_db`, tables, an administrator account, sample categories, and example plants. It is intended for a local development/demo database. Back up existing work before changing or re-running schema scripts on a non-empty database.

### 4. Configure the connection

Edit `dbconfig.ini` in the project folder. The file is copied to the output directory when the project builds.

```ini
Server=127.0.0.1
Port=3306
Database=fresh_farm_db
User=root
Password=YOUR_MYSQL_PASSWORD
```

Use the port that your MySQL server actually listens on (often 3306, but it may be another port). Use the password for your local MySQL account. Do not publish real database credentials to GitHub. For a shared or production deployment, create a least-privilege application account rather than using `root`.

### 5. Open and build

1. Open `FreshFarm.sln` in Visual Studio.
2. Set the configuration to **Debug** and platform to **x64**.
3. Build → **Build Solution**.
4. If the installed MSVC toolset does not include `v145`, open Project Properties → General and choose the installed toolset; update the `PlatformToolset` in both configurations if needed.
5. Run without debugging (Ctrl+F5).

### 6. Sign in

The SQL script seeds this local demo account:

- Username: `admin`
- Password: `Admin123!`

Change/remove demo credentials before using the application with real data. The current version authenticates users from the database; it does not yet include an in-app user-management/password-change screen.

## Troubleshooting

- **MySql.Data.dll not found:** install Connector/NET and correct the reference path described above.
- **Unable to connect / Access denied:** verify MySQL Server is running, the port and credentials in `dbconfig.ini` are correct, and `fresh_farm_db` exists.
- **Unknown database:** run `sql/plant_sales_schema.sql` first.
- **Cannot delete a category:** assign its plants to another category first.
- **Cannot delete a plant:** sales history references it, so the database blocks deletion to preserve reports.
- **No saleable plants appear:** add plants and stock in Plant Inventory first.
- **Toolset missing:** select an installed MSVC toolset in project properties; the exact toolset can vary by Visual Studio edition/update.

## Notes and scope

This project is designed as an educational local desktop application. It is not a certified production POS system. Before deployment, add a safer account provisioning/password-change workflow, role-based permission enforcement, audit logging, backups, stronger password storage (salted adaptive hashing), and comprehensive tests. The demo uses SHA-256 to avoid storing the password in plaintext, but SHA-256 alone is not recommended for production password storage.

The source was generated for this project and has not been compiled or run on your specific Windows installation in this environment. Treat the first build as a validation step; resolve any toolset/connector-specific configuration issues in Visual Studio.
