#pragma once
#include "Db.h"
#include <cmath>
using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::IO;

namespace FreshFarm {
	public ref class MainForm : public Form {
	private:
		int userId;
		String^ username;
		String^ fullName;
		String^ role;

		// Theme colors (Botanical Green)
		Color ColorForestGreen;      // #174F3B (24, 79, 59) - Primary dark
		Color ColorActionGreen;       // #237653 (35, 118, 83) - Primary action
		Color ColorLightGreen;        // #5DBE8F (93, 190, 143) - Accent
		Color ColorOffWhite;          // #FAFCFA (250, 252, 250) - Background
		Color ColorWhite;             // #FFFFFF - Card background
		Color ColorTextDark;          // #1F2D28 (31, 45, 40) - Dark text
		Color ColorTextMuted;         // #6B7B77 (107, 123, 119) - Muted text
		Color ColorError;             // #DC3C3C (220, 60, 60) - Error red
		Color ColorBorder;            // #E6EBE7 (230, 235, 231) - Subtle border
		Color ColorGridLine;          // Similar to border
		Color ColorSelectionLight;    // #DBE7E2 (219, 231, 226) - Light selection

		// Root layout structure
		TableLayoutPanel^ rootLayout;
		Panel^ mainContentHost;
		Panel^ pageContentPanel;
		Panel^ contentPanel;  // Alias for pageContentPanel for backward compatibility

		// Sales form controls (for event handling)
		ComboBox^ salesPlantCombo;
		NumericUpDown^ salesQtyControl;
		ComboBox^ salesPaymentCombo;

		// UI Layout
		Panel^ sidebar;
		Panel^ headerPanel;
		Label^ pageTitle;
		Label^ pageSubtitle;

		// Navigation buttons
		Button^ btnDashboard;
		Button^ btnPlants;
		Button^ btnCategories;
		Button^ btnSales;
		Button^ btnReports;
		Button^ btnLogout;

		// Track current page for proper cleanup
		String^ currentPage;

		// Page-specific controls (declared as fields to manage lifecycle)
		DataGridView^ plantsGrid;
		TextBox^ plantsSearchBox;
		DataGridView^ categoriesGrid;
		TextBox^ categoriesSearchBox;
		DataGridView^ lowStockGrid;
		DataGridView^ salesHistoryGrid;
		TextBox^ salesReportBox;

	public:
		MainForm(int uid, String^ uname, String^ fname, String^ r) {
			userId = uid;
			username = uname;
			fullName = fname;
			role = r;
			currentPage = "Dashboard";

			// Initialize theme colors
			ColorForestGreen = Color::FromArgb(24, 79, 59);
			ColorActionGreen = Color::FromArgb(35, 118, 83);
			ColorLightGreen = Color::FromArgb(93, 190, 143);
			ColorOffWhite = Color::FromArgb(250, 252, 250);
			ColorWhite = Color::White;
			ColorTextDark = Color::FromArgb(31, 45, 40);
			ColorTextMuted = Color::FromArgb(107, 123, 119);
			ColorError = Color::FromArgb(220, 60, 60);
			ColorBorder = Color::FromArgb(230, 235, 231);
			ColorGridLine = ColorBorder;
			ColorSelectionLight = Color::FromArgb(219, 231, 226);

			// Configure main form
			this->Text = "Fresh Farm - Plant Sales Management";
			this->StartPosition = FormStartPosition::CenterScreen;
			this->ClientSize = Drawing::Size(1280, 800);
			this->MinimumSize = Drawing::Size(1100, 700);
			this->BackColor = ColorOffWhite;
			this->Font = gcnew Drawing::Font("Segoe UI", 10);
			this->ShowIcon = false;

			// Create UI hierarchy: root layout first (creates sidebar, header, content panel)
			CreateRootLayout();
			// Old methods now redundant but called for safety
			CreateHeader();
			CreateContentPanel();

			// Wire navigation events
			btnDashboard->Click += gcnew EventHandler(this, &MainForm::OnDashboardNav);
			btnPlants->Click += gcnew EventHandler(this, &MainForm::OnPlantsNav);
			btnCategories->Click += gcnew EventHandler(this, &MainForm::OnCategoriesNav);
			btnSales->Click += gcnew EventHandler(this, &MainForm::OnSalesNav);
			btnReports->Click += gcnew EventHandler(this, &MainForm::OnReportsNav);

			// Handle resize events
			this->Resize += gcnew EventHandler(this, &MainForm::OnFormResize);

			// Initial page
			ShowDashboard();
		}

	private:
		void CreateRootLayout() {
			// Create root TableLayoutPanel with 2 columns: [180px sidebar] [Fill content]
			rootLayout = gcnew TableLayoutPanel();
			rootLayout->Dock = DockStyle::Fill;
			rootLayout->ColumnCount = 2;
			rootLayout->RowCount = 1;
			rootLayout->Padding = System::Windows::Forms::Padding(0);
			rootLayout->Margin = System::Windows::Forms::Padding(0);
			rootLayout->AutoSize = false;

			// Column style 0: Sidebar - fixed 180 pixels
			rootLayout->ColumnStyles->Clear();
			rootLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Absolute, 180.0f));
			// Column style 1: Main content - fill remaining
			rootLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0f));

			// Row style 0: Full height
			rootLayout->RowStyles->Clear();
			rootLayout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0f));

			this->Controls->Add(rootLayout);

			// Create sidebar and add to column 0
			CreateSidebar();
			rootLayout->Controls->Add(sidebar, 0, 0);

			// Create main content host in column 1
			mainContentHost = gcnew Panel();
			mainContentHost->BackColor = ColorOffWhite;
			mainContentHost->Dock = DockStyle::Fill;
			mainContentHost->Padding = System::Windows::Forms::Padding(0);
			mainContentHost->Margin = System::Windows::Forms::Padding(0);
			rootLayout->Controls->Add(mainContentHost, 1, 0);

			// Create header panel inside main content host
			headerPanel = gcnew Panel();
			headerPanel->BackColor = ColorWhite;
			headerPanel->Dock = DockStyle::Top;
			headerPanel->Height = 80;
			headerPanel->Padding = System::Windows::Forms::Padding(20);
			headerPanel->Margin = System::Windows::Forms::Padding(0);
			mainContentHost->Controls->Add(headerPanel);

			// Page title
			pageTitle = gcnew Label();
			pageTitle->Text = "Dashboard";
			pageTitle->Font = gcnew Drawing::Font("Segoe UI Semibold", 22, FontStyle::Bold);
			pageTitle->ForeColor = ColorTextDark;
			pageTitle->AutoSize = true;
			pageTitle->Dock = DockStyle::Top;
			headerPanel->Controls->Add(pageTitle);

			// Page subtitle
			pageSubtitle = gcnew Label();
			pageSubtitle->Text = "Overview of store activity";
			pageSubtitle->Font = gcnew Drawing::Font("Segoe UI", 10);
			pageSubtitle->ForeColor = ColorTextMuted;
			pageSubtitle->AutoSize = true;
			pageSubtitle->Dock = DockStyle::Top;
			headerPanel->Controls->Add(pageSubtitle);

			// Create page content panel inside main content host, below header
			pageContentPanel = gcnew Panel();
			pageContentPanel->BackColor = ColorOffWhite;
			pageContentPanel->Dock = DockStyle::Fill;
			pageContentPanel->Padding = System::Windows::Forms::Padding(0, 20, 20, 20);
			pageContentPanel->Margin = System::Windows::Forms::Padding(0);
			pageContentPanel->AutoScroll = true;
			mainContentHost->Controls->Add(pageContentPanel);

			// contentPanel is an alias for pageContentPanel for backward compatibility
			contentPanel = pageContentPanel;
		}

		void CreateSidebar() {
			sidebar = gcnew Panel();
			sidebar->BackColor = ColorForestGreen;
			sidebar->Dock = DockStyle::Fill;
			sidebar->Padding = System::Windows::Forms::Padding(0);
			sidebar->Margin = System::Windows::Forms::Padding(0);
			// Will be added to rootLayout column 0, not to main form directly

			// Brand header
			Label^ brandLabel = gcnew Label();
			brandLabel->Text = "FRESH\nFARM";
			brandLabel->ForeColor = ColorWhite;
			brandLabel->Font = gcnew Drawing::Font("Segoe UI", 14, FontStyle::Bold);
			brandLabel->TextAlign = ContentAlignment::TopCenter;
			brandLabel->Location = Point(8, 20);
			brandLabel->Size = Drawing::Size(164, 84);
			sidebar->Controls->Add(brandLabel);

			// Create navigation buttons
			int navTop = 120;
			const int buttonHeight = 44;
			const int buttonMargin = 6;
			const int buttonWidth = 164;

			btnDashboard = CreateNavButton("Dashboard", navTop);
			navTop += buttonHeight + buttonMargin;
			sidebar->Controls->Add(btnDashboard);

			btnPlants = CreateNavButton("Plants", navTop);
			navTop += buttonHeight + buttonMargin;
			sidebar->Controls->Add(btnPlants);

			btnCategories = CreateNavButton("Categories", navTop);
			navTop += buttonHeight + buttonMargin;
			sidebar->Controls->Add(btnCategories);

			btnSales = CreateNavButton("Sales", navTop);
			navTop += buttonHeight + buttonMargin;
			sidebar->Controls->Add(btnSales);

			btnReports = CreateNavButton("Reports", navTop);
			navTop += buttonHeight + buttonMargin;
			sidebar->Controls->Add(btnReports);

			// Logout button at bottom
			btnLogout = gcnew Button();
			btnLogout->Text = "Sign Out";
			btnLogout->Size = Drawing::Size(164, 44);
			btnLogout->Location = Point(8, sidebar->ClientSize.Height - 56);
			btnLogout->Anchor = AnchorStyles::Bottom | AnchorStyles::Left;
			btnLogout->BackColor = ColorError;
			btnLogout->ForeColor = ColorWhite;
			btnLogout->FlatStyle = FlatStyle::Flat;
			btnLogout->FlatAppearance->BorderSize = 0;
			btnLogout->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnLogout->Cursor = Cursors::Hand;
			btnLogout->Click += gcnew EventHandler(this, &MainForm::OnLogout);
			sidebar->Controls->Add(btnLogout);
			}

		Button^ CreateNavButton(String^ text, int top) {
			Button^ btn = gcnew Button();
			btn->Text = text;
			btn->Location = Point(8, top);
			btn->Size = Drawing::Size(164, 44);
			btn->BackColor = ColorForestGreen;
			btn->ForeColor = Color::FromArgb(224, 240, 230);
			btn->FlatStyle = FlatStyle::Flat;
			btn->FlatAppearance->BorderSize = 0;
			btn->Font = gcnew Drawing::Font("Segoe UI", 10);
			btn->Cursor = Cursors::Hand;
			btn->TextAlign = ContentAlignment::MiddleLeft;
			return btn;
		}

		void CreateHeader() {
			// Header creation is now handled in CreateRootLayout
			// This method is kept for backward compatibility but does nothing
		}

		void CreateContentPanel() {
			// Page content panel creation is now handled in CreateRootLayout
			// contentPanel is set as an alias to pageContentPanel
			// This method is kept for backward compatibility but does nothing
		}

		void OnFormResize(Object^ sender, EventArgs^ e) {
			// Only refresh dashboard cards when resizing (they recalculate sizes)
			// Don't rebuild entire page to avoid flickering
			if (currentPage == "Dashboard") {
				// Rebuild dashboard to recalculate card widths
				ShowDashboard();
			}
		}

		void PreparePage(String^ title, String^ subtitle) {
			contentPanel->Controls->Clear();
			pageTitle->Text = title;
			pageSubtitle->Text = subtitle;
			currentPage = title;
		}

		// ============================================================================
		// DASHBOARD PAGE
		// ============================================================================

		void ShowDashboard() {
			PreparePage("Dashboard", "Overview of store activity");
			try {
				// Query statistics
				Object^ plantCountObj = Db::Scalar("SELECT COUNT(*) FROM plants", gcnew array<Object^>(0));
				Object^ categoryCountObj = Db::Scalar("SELECT COUNT(*) FROM categories", gcnew array<Object^>(0));
				Object^ todaySalesObj = Db::Scalar("SELECT COALESCE(SUM(TotalAmount),0) FROM sales WHERE DATE(SaleDate)=CURDATE()", gcnew array<Object^>(0));
				Object^ lowStockObj = Db::Scalar("SELECT COUNT(*) FROM plants WHERE StockQuantity <= 5", gcnew array<Object^>(0));

				int plantCount = Convert::ToInt32(plantCountObj);
				int categoryCount = Convert::ToInt32(categoryCountObj);
				Decimal todayRevenue = Convert::ToDecimal(todaySalesObj);
				int lowStockCount = Convert::ToInt32(lowStockObj);

				// Create stat cards using TableLayoutPanel for precise layout
				// Calculate equal width for 4 cards with spacing
				int totalMargins = 16 * 5;  // Left/right padding (16) + 4 inter-card margins (6 each = 24, but first has none, last has none = 5 margins of 6 = 30, but accounting for container margin)
				int availableWidth = contentPanel->ClientSize.Width - 40;  // 20px padding on each side
				int cardWidth = (availableWidth - 24) / 4;  // 24px for margins between 4 cards
				if (cardWidth < 140) cardWidth = 140;  // Minimum width

				// Create TableLayoutPanel for 4 cards in one row
				TableLayoutPanel^ cardsPanel = gcnew TableLayoutPanel();
				cardsPanel->BackColor = ColorOffWhite;
				cardsPanel->Dock = DockStyle::Top;
				cardsPanel->Height = 165;
				cardsPanel->AutoSize = false;
				cardsPanel->ColumnCount = 4;
				cardsPanel->RowCount = 1;
				cardsPanel->Padding = System::Windows::Forms::Padding(0);
				cardsPanel->Margin = System::Windows::Forms::Padding(0, 0, 0, 24);  // 24px space below cards
				cardsPanel->GrowStyle = TableLayoutPanelGrowStyle::FixedSize;

				// Set equal column widths
				for (int i = 0; i < 4; i++) {
					cardsPanel->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 25.0f));
				}
				cardsPanel->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0f));

				Panel^ card1 = CreateStatCard("Total Plants", String::Format("{0}", plantCount), ColorActionGreen);
				Panel^ card2 = CreateStatCard("Categories", String::Format("{0}", categoryCount), ColorForestGreen);
				Panel^ card3 = CreateStatCard("Today's Revenue", String::Format("Rs. {0:0.00}", todayRevenue), ColorLightGreen);
				Panel^ card4 = CreateStatCard("Low Stock Items", String::Format("{0}", lowStockCount), ColorError);

				cardsPanel->Controls->Add(card1, 0, 0);
				cardsPanel->Controls->Add(card2, 1, 0);
				cardsPanel->Controls->Add(card3, 2, 0);
				cardsPanel->Controls->Add(card4, 3, 0);

				contentPanel->Controls->Add(cardsPanel);

				// Low-stock items section
				Label^ lowStockLabel = gcnew Label();
				lowStockLabel->Text = "Low-Stock Items (≤5 units):";
				lowStockLabel->Font = gcnew Drawing::Font("Segoe UI Semibold", 11, FontStyle::Bold);
				lowStockLabel->ForeColor = ColorTextDark;
				lowStockLabel->AutoSize = true;
				lowStockLabel->Margin = System::Windows::Forms::Padding(0, 16, 0, 8);
				lowStockLabel->Dock = DockStyle::Top;
				contentPanel->Controls->Add(lowStockLabel);

				// Query low-stock items
				DataTable^ lowStockData = Db::Query("SELECT PlantName, StockQuantity FROM plants WHERE StockQuantity <= 5 ORDER BY StockQuantity ASC", gcnew array<Object^>(0));

				if (lowStockData->Rows->Count == 0) {
					// Empty state message
					Label^ emptyMsg = gcnew Label();
					emptyMsg->Text = "No low-stock items at this time.";
					emptyMsg->ForeColor = ColorTextMuted;
					emptyMsg->Font = gcnew Drawing::Font("Segoe UI", 10);
					emptyMsg->Dock = DockStyle::Fill;
					emptyMsg->Padding = System::Windows::Forms::Padding(16);
					contentPanel->Controls->Add(emptyMsg);
				} else {
					// Low-stock grid
					lowStockGrid = gcnew DataGridView();
					lowStockGrid->BackgroundColor = ColorWhite;
					lowStockGrid->GridColor = ColorGridLine;
					lowStockGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
					lowStockGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
					lowStockGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
					lowStockGrid->ColumnHeadersHeight = 36;
					lowStockGrid->RowTemplate->Height = 28;
					lowStockGrid->ReadOnly = true;
					lowStockGrid->AllowUserToAddRows = false;
					lowStockGrid->RowHeadersVisible = false;
					lowStockGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
					lowStockGrid->MultiSelect = false;
					lowStockGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
					lowStockGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
					lowStockGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
					lowStockGrid->Dock = DockStyle::Fill;
					lowStockGrid->DataSource = lowStockData;
					contentPanel->Controls->Add(lowStockGrid);
				}
			}
			catch (Exception^ ex) {
				Label^ errorLabel = gcnew Label();
				errorLabel->Text = "Error loading dashboard: " + ex->Message;
				errorLabel->ForeColor = ColorError;
				errorLabel->AutoSize = true;
				errorLabel->Dock = DockStyle::Top;
				errorLabel->Padding = System::Windows::Forms::Padding(16);
				contentPanel->Controls->Add(errorLabel);
			}
		}

		Panel^ CreateStatCard(String^ title, String^ value, Color bgColor) {
			Panel^ card = gcnew Panel();
			card->BackColor = bgColor;
			card->Padding = System::Windows::Forms::Padding(12);
			card->Margin = System::Windows::Forms::Padding(6);
			card->Dock = DockStyle::Fill;
			card->Height = 145;

			// Use TableLayoutPanel for title and value with proper spacing
			TableLayoutPanel^ cardLayout = gcnew TableLayoutPanel();
			cardLayout->Dock = DockStyle::Fill;
			cardLayout->BackColor = bgColor;
			cardLayout->RowCount = 2;
			cardLayout->ColumnCount = 1;
			cardLayout->Padding = System::Windows::Forms::Padding(0);
			cardLayout->Margin = System::Windows::Forms::Padding(0);
			cardLayout->GrowStyle = TableLayoutPanelGrowStyle::FixedSize;

			// Row 0: Title (Auto height)
			cardLayout->RowStyles->Add(gcnew RowStyle(SizeType::AutoSize));
			// Row 1: Value (Fill remaining space)
			cardLayout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100.0f));
			cardLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 100.0f));

			Label^ titleLabel = gcnew Label();
			titleLabel->Text = title;
			titleLabel->ForeColor = ColorWhite;
			titleLabel->Font = gcnew Drawing::Font("Segoe UI", 10);
			titleLabel->Dock = DockStyle::Fill;
			titleLabel->Margin = System::Windows::Forms::Padding(0, 0, 0, 8);
			cardLayout->Controls->Add(titleLabel, 0, 0);

			Label^ valueLabel = gcnew Label();
			valueLabel->Text = value;
			valueLabel->ForeColor = ColorWhite;
			valueLabel->Font = gcnew Drawing::Font("Segoe UI Semibold", 22, FontStyle::Bold);
			valueLabel->Dock = DockStyle::Fill;
			valueLabel->AutoSize = false;
			valueLabel->TextAlign = ContentAlignment::BottomLeft;
			cardLayout->Controls->Add(valueLabel, 0, 1);

			card->Controls->Add(cardLayout);
			return card;
		}

		// ============================================================================
		// PLANT EDITOR FORM
		// ============================================================================

		ref class PlantEditorForm : public Form {
		public:
			int EditingPlantId;
			String^ PlantName;
			int CategoryId;
			Decimal Price;
			int StockQuantity;
			String^ Description;

		private:
			TextBox^ txtPlantName;
			ComboBox^ cbCategory;
			NumericUpDown^ nudPrice;
			NumericUpDown^ nudStock;
			TextBox^ txtDescription;

		public:
			PlantEditorForm(int plantId) {
				EditingPlantId = plantId;
				this->Text = (plantId == 0) ? "Add Plant" : "Edit Plant";
				this->ClientSize = Drawing::Size(500, 420);
				this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
				this->MaximizeBox = false;
				this->MinimizeBox = false;
				this->StartPosition = FormStartPosition::CenterParent;

				int yPos = 16;

				// Plant Name
				Label^ lblName = gcnew Label();
				lblName->Text = "Plant Name *";
				lblName->Location = Point(16, yPos);
				lblName->AutoSize = true;
				this->Controls->Add(lblName);
				yPos += 24;

				txtPlantName = gcnew TextBox();
				txtPlantName->Location = Point(16, yPos);
				txtPlantName->Width = 460;
				txtPlantName->Height = 32;
				this->Controls->Add(txtPlantName);
				yPos += 40;

				// Category
				Label^ lblCategory = gcnew Label();
				lblCategory->Text = "Category *";
				lblCategory->Location = Point(16, yPos);
				lblCategory->AutoSize = true;
				this->Controls->Add(lblCategory);
				yPos += 24;

				cbCategory = gcnew ComboBox();
				cbCategory->Location = Point(16, yPos);
				cbCategory->Width = 460;
				cbCategory->Height = 32;
				cbCategory->DropDownStyle = ComboBoxStyle::DropDownList;
				this->Controls->Add(cbCategory);

				try {
					DataTable^ cats = Db::Query("SELECT CategoryId, CategoryName FROM categories ORDER BY CategoryName", gcnew array<Object^>(0));
					cbCategory->DataSource = cats;
					cbCategory->DisplayMember = "CategoryName";
					cbCategory->ValueMember = "CategoryId";
				}
				catch (...) { }
				yPos += 40;

				// Price and Stock (two columns)
				Label^ lblPrice = gcnew Label();
				lblPrice->Text = "Price (Rs.) *";
				lblPrice->Location = Point(16, yPos);
				lblPrice->AutoSize = true;
				this->Controls->Add(lblPrice);

				Label^ lblStock = gcnew Label();
				lblStock->Text = "Stock Qty *";
				lblStock->Location = Point(250, yPos);
				lblStock->AutoSize = true;
				this->Controls->Add(lblStock);
				yPos += 24;

				nudPrice = gcnew NumericUpDown();
				nudPrice->Location = Point(16, yPos);
				nudPrice->Width = 220;
				nudPrice->DecimalPlaces = 2;
				nudPrice->Maximum = Decimal(999999);
				nudPrice->Height = 32;
				this->Controls->Add(nudPrice);

				nudStock = gcnew NumericUpDown();
				nudStock->Location = Point(250, yPos);
				nudStock->Width = 226;
				nudStock->Maximum = 100000;
				nudStock->Height = 32;
				this->Controls->Add(nudStock);
				yPos += 40;

				// Description
				Label^ lblDesc = gcnew Label();
				lblDesc->Text = "Description";
				lblDesc->Location = Point(16, yPos);
				lblDesc->AutoSize = true;
				this->Controls->Add(lblDesc);
				yPos += 24;

				txtDescription = gcnew TextBox();
				txtDescription->Location = Point(16, yPos);
				txtDescription->Width = 460;
				txtDescription->Height = 80;
				txtDescription->Multiline = true;
				txtDescription->AcceptsReturn = true;
				this->Controls->Add(txtDescription);
				yPos += 88;

				// OK button
				Button^ btnOK = gcnew Button();
				btnOK->Text = "Save";
				btnOK->Location = Point(330, yPos);
				btnOK->Width = 70;
				btnOK->Height = 36;
				btnOK->DialogResult = System::Windows::Forms::DialogResult::OK;
				btnOK->Click += gcnew EventHandler(this, &PlantEditorForm::OnOK);
				this->Controls->Add(btnOK);

				// Cancel button
				Button^ btnCancel = gcnew Button();
				btnCancel->Text = "Cancel";
				btnCancel->Location = Point(406, yPos);
				btnCancel->Width = 70;
				btnCancel->Height = 36;
				btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->Controls->Add(btnCancel);

				if (plantId != 0) {
					LoadExistingPlant();
				}
			}

		private:
			void LoadExistingPlant() {
				try {
					DataTable^ dt = Db::Query("SELECT PlantName, CategoryId, Price, StockQuantity, Description FROM plants WHERE PlantId=@p0", gcnew array<Object^> { EditingPlantId });
					if (dt->Rows->Count > 0) {
						DataRow^ r = dt->Rows[0];
						txtPlantName->Text = Convert::ToString(r["PlantName"]);
						cbCategory->SelectedValue = Convert::ToInt32(r["CategoryId"]);
						nudPrice->Value = Convert::ToDecimal(r["Price"]);
						nudStock->Value = Convert::ToInt32(r["StockQuantity"]);
						txtDescription->Text = (r["Description"] == DBNull::Value) ? "" : Convert::ToString(r["Description"]);
					}
				}
				catch (...) { }
			}

			void OnOK(Object^ sender, EventArgs^ e) {
				// Validate
				if (String::IsNullOrWhiteSpace(txtPlantName->Text)) {
					MessageBox::Show("Plant name is required.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					this->DialogResult = System::Windows::Forms::DialogResult::None;
					return;
				}
				if (cbCategory->SelectedValue == nullptr) {
					MessageBox::Show("Please select a category.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					this->DialogResult = System::Windows::Forms::DialogResult::None;
					return;
				}
				if (nudPrice->Value <= 0) {
					MessageBox::Show("Price must be greater than zero.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					this->DialogResult = System::Windows::Forms::DialogResult::None;
					return;
				}

				PlantName = txtPlantName->Text->Trim();
				CategoryId = Convert::ToInt32(cbCategory->SelectedValue);
				Price = nudPrice->Value;
				StockQuantity = Convert::ToInt32(nudStock->Value);
				Description = txtDescription->Text->Trim();
			}
		};

		// ============================================================================
		// PLANTS PAGE
		// ============================================================================

		void LoadPlants(String^ searchText) {
			try {
				String^ query = "SELECT PlantId, PlantName, c.CategoryName, Price, StockQuantity, p.Description "
					"FROM plants p LEFT JOIN categories c ON p.CategoryId = c.CategoryId "
					"WHERE p.PlantName LIKE @p0 OR p.Description LIKE @p0 "
					"ORDER BY p.PlantName";
				String^ pattern = "%" + (String::IsNullOrEmpty(searchText) ? "" : searchText) + "%";
				DataTable^ dt = Db::Query(query, gcnew array<Object^> { pattern });
				plantsGrid->DataSource = dt;

				// Hide PlantId column
				if (plantsGrid->Columns->Contains("PlantId")) {
					plantsGrid->Columns["PlantId"]->Visible = false;
				}

				// Adjust column widths
				if (plantsGrid->Columns->Contains("PlantName")) {
					plantsGrid->Columns["PlantName"]->Width = 180;
				}
				if (plantsGrid->Columns->Contains("Price")) {
					plantsGrid->Columns["Price"]->Width = 100;
				}
				if (plantsGrid->Columns->Contains("StockQuantity")) {
					plantsGrid->Columns["StockQuantity"]->Width = 80;
				}
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error loading plants: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		void ShowPlants() {
			PreparePage("Plants", "Manage plant inventory, pricing and stock");

			// Top toolbar panel
			Panel^ toolbar = gcnew Panel();
			toolbar->BackColor = ColorOffWhite;
			toolbar->Dock = DockStyle::Top;
			toolbar->Height = 56;
			toolbar->Padding = System::Windows::Forms::Padding(0, 8, 0, 8);
			toolbar->AutoSize = false;
			contentPanel->Controls->Add(toolbar);

			// Use FlowLayoutPanel for toolbar items to avoid overlapping
			FlowLayoutPanel^ toolFlow = gcnew FlowLayoutPanel();
			toolFlow->Dock = DockStyle::Fill;
			toolFlow->AutoSize = false;
			toolFlow->Padding = System::Windows::Forms::Padding(0);
			toolFlow->Margin = System::Windows::Forms::Padding(0);
			toolFlow->WrapContents = false;
			toolbar->Controls->Add(toolFlow);

			// Search box
			plantsSearchBox = gcnew TextBox();
			plantsSearchBox->Size = Drawing::Size(280, 36);
			plantsSearchBox->Font = gcnew Drawing::Font("Segoe UI", 10);
			plantsSearchBox->BorderStyle = BorderStyle::FixedSingle;
			plantsSearchBox->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			plantsSearchBox->Text = "";
			toolFlow->Controls->Add(plantsSearchBox);

			// Search button
			Button^ btnSearch = gcnew Button();
			btnSearch->Text = "Search";
			btnSearch->Size = Drawing::Size(90, 36);
			btnSearch->BackColor = ColorActionGreen;
			btnSearch->ForeColor = ColorWhite;
			btnSearch->FlatStyle = FlatStyle::Flat;
			btnSearch->FlatAppearance->BorderSize = 0;
			btnSearch->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnSearch->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnSearch->Click += gcnew EventHandler(this, &MainForm::OnPlantsSearch);
			toolFlow->Controls->Add(btnSearch);

			// Add button
			Button^ btnAddPlant = gcnew Button();
			btnAddPlant->Text = "+ Add Plant";
			btnAddPlant->Size = Drawing::Size(110, 36);
			btnAddPlant->BackColor = ColorLightGreen;
			btnAddPlant->ForeColor = ColorWhite;
			btnAddPlant->FlatStyle = FlatStyle::Flat;
			btnAddPlant->FlatAppearance->BorderSize = 0;
			btnAddPlant->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnAddPlant->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnAddPlant->Click += gcnew EventHandler(this, &MainForm::OnAddPlant);
			toolFlow->Controls->Add(btnAddPlant);

			// Edit button
			Button^ btnEditPlant = gcnew Button();
			btnEditPlant->Text = "Edit";
			btnEditPlant->Size = Drawing::Size(80, 36);
			btnEditPlant->BackColor = ColorTextMuted;
			btnEditPlant->ForeColor = ColorWhite;
			btnEditPlant->FlatStyle = FlatStyle::Flat;
			btnEditPlant->FlatAppearance->BorderSize = 0;
			btnEditPlant->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnEditPlant->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnEditPlant->Click += gcnew EventHandler(this, &MainForm::OnEditPlant);
			toolFlow->Controls->Add(btnEditPlant);

			// Delete button
			Button^ btnDeletePlant = gcnew Button();
			btnDeletePlant->Text = "Delete";
			btnDeletePlant->Size = Drawing::Size(80, 36);
			btnDeletePlant->BackColor = ColorError;
			btnDeletePlant->ForeColor = ColorWhite;
			btnDeletePlant->FlatStyle = FlatStyle::Flat;
			btnDeletePlant->FlatAppearance->BorderSize = 0;
			btnDeletePlant->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnDeletePlant->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnDeletePlant->Click += gcnew EventHandler(this, &MainForm::OnDeletePlant);
			toolFlow->Controls->Add(btnDeletePlant);

			// Plants grid
			plantsGrid = gcnew DataGridView();
			plantsGrid->BackgroundColor = ColorWhite;
			plantsGrid->GridColor = ColorGridLine;
			plantsGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
			plantsGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
			plantsGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
			plantsGrid->ColumnHeadersHeight = 36;
			plantsGrid->RowTemplate->Height = 28;
			plantsGrid->ReadOnly = true;
			plantsGrid->AllowUserToAddRows = false;
			plantsGrid->RowHeadersVisible = false;
			plantsGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
			plantsGrid->MultiSelect = false;
			plantsGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
			plantsGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
			plantsGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
			plantsGrid->Dock = DockStyle::Fill;
			contentPanel->Controls->Add(plantsGrid);

			LoadPlants("");
		}

		void OnPlantsSearch(Object^ sender, EventArgs^ e) {
			LoadPlants(plantsSearchBox->Text->Trim());
		}

		void OnAddPlant(Object^ sender, EventArgs^ e) {
			PlantEditorForm^ editor = gcnew PlantEditorForm(0);
			if (editor->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				try {
					String^ sql = "INSERT INTO plants (PlantName, CategoryId, Price, StockQuantity, Description) VALUES (@p0, @p1, @p2, @p3, @p4)";
					array<Object^>^ params = gcnew array<Object^> { 
						editor->PlantName, 
						editor->CategoryId, 
						editor->Price, 
						editor->StockQuantity, 
						editor->Description 
					};
					Db::Execute(sql, params);
					LoadPlants("");
				}
				catch (Exception^ ex) {
					MessageBox::Show("Error adding plant: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
		}

		void OnEditPlant(Object^ sender, EventArgs^ e) {
			if (plantsGrid->SelectedRows->Count == 0) {
				MessageBox::Show("Please select a plant to edit.", "Edit Plant", MessageBoxButtons::OK, MessageBoxIcon::Information);
				return;
			}
			DataRowView^ row = (DataRowView^)plantsGrid->SelectedRows[0]->DataBoundItem;
			int plantId = Convert::ToInt32(row["PlantId"]);

			PlantEditorForm^ editor = gcnew PlantEditorForm(plantId);
			if (editor->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				try {
					String^ sql = "UPDATE plants SET PlantName=@p0, CategoryId=@p1, Price=@p2, StockQuantity=@p3, Description=@p4 WHERE PlantId=@p5";
					array<Object^>^ params = gcnew array<Object^> { 
						editor->PlantName, 
						editor->CategoryId, 
						editor->Price, 
						editor->StockQuantity, 
						editor->Description, 
						plantId 
					};
					Db::Execute(sql, params);
					LoadPlants(plantsSearchBox->Text->Trim());
				}
				catch (Exception^ ex) {
					MessageBox::Show("Error updating plant: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
		}

		void OnDeletePlant(Object^ sender, EventArgs^ e) {
			if (plantsGrid->SelectedRows->Count == 0) {
				MessageBox::Show("Please select a plant to delete.", "Delete Plant", MessageBoxButtons::OK, MessageBoxIcon::Information);
				return;
			}
			DataRowView^ row = (DataRowView^)plantsGrid->SelectedRows[0]->DataBoundItem;
			int plantId = Convert::ToInt32(row["PlantId"]);
			String^ plantName = Convert::ToString(row["PlantName"]);

			if (MessageBox::Show("Delete '" + plantName + "'? This action cannot be undone.", "Confirm Deletion", MessageBoxButtons::YesNo, MessageBoxIcon::Warning) != System::Windows::Forms::DialogResult::Yes) {
				return;
			}

			try {
				Db::Execute("DELETE FROM plants WHERE PlantId=@p0", gcnew array<Object^> { plantId });
				LoadPlants(plantsSearchBox->Text->Trim());
				MessageBox::Show("Plant deleted successfully.", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error deleting plant: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		// ============================================================================
		// CATEGORIES PAGE
		// ============================================================================

		void LoadCategories(String^ searchText) {
			try {
				String^ query = "SELECT CategoryId, CategoryName, Description FROM categories WHERE CategoryName LIKE @p0 ORDER BY CategoryName";
				String^ pattern = "%" + (String::IsNullOrEmpty(searchText) ? "" : searchText) + "%";
				DataTable^ dt = Db::Query(query, gcnew array<Object^> { pattern });
				categoriesGrid->DataSource = dt;

				if (categoriesGrid->Columns->Contains("CategoryId")) {
					categoriesGrid->Columns["CategoryId"]->Visible = false;
				}
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error loading categories: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		void ShowCategories() {
			PreparePage("Categories", "Manage product categories");

			// Top toolbar panel
			Panel^ toolbar = gcnew Panel();
			toolbar->BackColor = ColorOffWhite;
			toolbar->Dock = DockStyle::Top;
			toolbar->Height = 56;
			toolbar->Padding = System::Windows::Forms::Padding(0, 8, 0, 8);
			toolbar->AutoSize = false;
			contentPanel->Controls->Add(toolbar);

			// Use FlowLayoutPanel for toolbar items
			FlowLayoutPanel^ toolFlow = gcnew FlowLayoutPanel();
			toolFlow->Dock = DockStyle::Fill;
			toolFlow->AutoSize = false;
			toolFlow->Padding = System::Windows::Forms::Padding(0);
			toolFlow->Margin = System::Windows::Forms::Padding(0);
			toolFlow->WrapContents = false;
			toolbar->Controls->Add(toolFlow);

			// Search box
			categoriesSearchBox = gcnew TextBox();
			categoriesSearchBox->Size = Drawing::Size(280, 36);
			categoriesSearchBox->Font = gcnew Drawing::Font("Segoe UI", 10);
			categoriesSearchBox->BorderStyle = BorderStyle::FixedSingle;
			categoriesSearchBox->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			categoriesSearchBox->Text = "";
			toolFlow->Controls->Add(categoriesSearchBox);

			// Search button
			Button^ btnSearch = gcnew Button();
			btnSearch->Text = "Search";
			btnSearch->Size = Drawing::Size(90, 36);
			btnSearch->BackColor = ColorActionGreen;
			btnSearch->ForeColor = ColorWhite;
			btnSearch->FlatStyle = FlatStyle::Flat;
			btnSearch->FlatAppearance->BorderSize = 0;
			btnSearch->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnSearch->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnSearch->Click += gcnew EventHandler(this, &MainForm::OnCategoriesSearch);
			toolFlow->Controls->Add(btnSearch);

			// Add button
			Button^ btnAdd = gcnew Button();
			btnAdd->Text = "+ Add Category";
			btnAdd->Size = Drawing::Size(130, 36);
			btnAdd->BackColor = ColorLightGreen;
			btnAdd->ForeColor = ColorWhite;
			btnAdd->FlatStyle = FlatStyle::Flat;
			btnAdd->FlatAppearance->BorderSize = 0;
			btnAdd->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnAdd->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnAdd->Click += gcnew EventHandler(this, &MainForm::OnAddCategory);
			toolFlow->Controls->Add(btnAdd);

			// Edit button
			Button^ btnEdit = gcnew Button();
			btnEdit->Text = "Edit";
			btnEdit->Size = Drawing::Size(80, 36);
			btnEdit->BackColor = ColorTextMuted;
			btnEdit->ForeColor = ColorWhite;
			btnEdit->FlatStyle = FlatStyle::Flat;
			btnEdit->FlatAppearance->BorderSize = 0;
			btnEdit->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnEdit->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnEdit->Click += gcnew EventHandler(this, &MainForm::OnEditCategory);
			toolFlow->Controls->Add(btnEdit);

			// Delete button
			Button^ btnDelete = gcnew Button();
			btnDelete->Text = "Delete";
			btnDelete->Size = Drawing::Size(80, 36);
			btnDelete->BackColor = ColorError;
			btnDelete->ForeColor = ColorWhite;
			btnDelete->FlatStyle = FlatStyle::Flat;
			btnDelete->FlatAppearance->BorderSize = 0;
			btnDelete->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnDelete->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			btnDelete->Click += gcnew EventHandler(this, &MainForm::OnDeleteCategory);
			toolFlow->Controls->Add(btnDelete);

			// Categories grid
			categoriesGrid = gcnew DataGridView();
			categoriesGrid->BackgroundColor = ColorWhite;
			categoriesGrid->GridColor = ColorGridLine;
			categoriesGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
			categoriesGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
			categoriesGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
			categoriesGrid->ColumnHeadersHeight = 36;
			categoriesGrid->RowTemplate->Height = 28;
			categoriesGrid->ReadOnly = true;
			categoriesGrid->AllowUserToAddRows = false;
			categoriesGrid->RowHeadersVisible = false;
			categoriesGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
			categoriesGrid->MultiSelect = false;
			categoriesGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
			categoriesGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
			categoriesGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
			categoriesGrid->Dock = DockStyle::Fill;
			contentPanel->Controls->Add(categoriesGrid);

			LoadCategories("");
		}

		void OnCategoriesSearch(Object^ sender, EventArgs^ e) {
			LoadCategories(categoriesSearchBox->Text->Trim());
		}

		void OnAddCategory(Object^ sender, EventArgs^ e) {
			Form^ dialog = gcnew Form();
			dialog->Text = "Add Category";
			dialog->ClientSize = Drawing::Size(420, 240);
			dialog->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			dialog->StartPosition = FormStartPosition::CenterParent;

			Label^ lbl = gcnew Label(); lbl->Text = "Category Name *"; lbl->Location = Point(16, 16); lbl->AutoSize = true; dialog->Controls->Add(lbl);
			TextBox^ txtName = gcnew TextBox(); txtName->Location = Point(16, 40); txtName->Width = 388; txtName->Height = 32; dialog->Controls->Add(txtName);

			lbl = gcnew Label(); lbl->Text = "Description"; lbl->Location = Point(16, 80); lbl->AutoSize = true; dialog->Controls->Add(lbl);
			TextBox^ txtDesc = gcnew TextBox(); txtDesc->Location = Point(16, 104); txtDesc->Width = 388; txtDesc->Height = 80; txtDesc->Multiline = true; dialog->Controls->Add(txtDesc);

			Button^ btnOK = gcnew Button(); btnOK->Text = "Save"; btnOK->Location = Point(263, 200); btnOK->Width = 70; btnOK->Height = 32; btnOK->DialogResult = System::Windows::Forms::DialogResult::OK; dialog->Controls->Add(btnOK);
			Button^ btnCancel = gcnew Button(); btnCancel->Text = "Cancel"; btnCancel->Location = Point(342, 200); btnCancel->Width = 70; btnCancel->Height = 32; btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel; dialog->Controls->Add(btnCancel);

			if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				if (String::IsNullOrWhiteSpace(txtName->Text)) {
					MessageBox::Show("Category name is required.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					return;
				}
				try {
					Db::Execute("INSERT INTO categories (CategoryName, Description) VALUES (@p0, @p1)", gcnew array<Object^> { txtName->Text->Trim(), txtDesc->Text->Trim() });
					LoadCategories("");
					MessageBox::Show("Category added successfully.", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
				catch (Exception^ ex) {
					MessageBox::Show("Error: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
		}

		void OnEditCategory(Object^ sender, EventArgs^ e) {
			if (categoriesGrid->SelectedRows->Count == 0) {
				MessageBox::Show("Please select a category to edit.", "Edit Category", MessageBoxButtons::OK, MessageBoxIcon::Information);
				return;
			}
			DataRowView^ row = (DataRowView^)categoriesGrid->SelectedRows[0]->DataBoundItem;
			int catId = Convert::ToInt32(row["CategoryId"]);
			String^ catName = Convert::ToString(row["CategoryName"]);
			String^ catDesc = (row["Description"] == DBNull::Value) ? "" : Convert::ToString(row["Description"]);

			Form^ dialog = gcnew Form();
			dialog->Text = "Edit Category";
			dialog->ClientSize = Drawing::Size(420, 240);
			dialog->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			dialog->StartPosition = FormStartPosition::CenterParent;

			Label^ lbl = gcnew Label(); lbl->Text = "Category Name *"; lbl->Location = Point(16, 16); lbl->AutoSize = true; dialog->Controls->Add(lbl);
			TextBox^ txtName = gcnew TextBox(); txtName->Text = catName; txtName->Location = Point(16, 40); txtName->Width = 388; txtName->Height = 32; dialog->Controls->Add(txtName);

			lbl = gcnew Label(); lbl->Text = "Description"; lbl->Location = Point(16, 80); lbl->AutoSize = true; dialog->Controls->Add(lbl);
			TextBox^ txtDesc = gcnew TextBox(); txtDesc->Text = catDesc; txtDesc->Location = Point(16, 104); txtDesc->Width = 388; txtDesc->Height = 80; txtDesc->Multiline = true; dialog->Controls->Add(txtDesc);

			Button^ btnOK = gcnew Button(); btnOK->Text = "Save"; btnOK->Location = Point(263, 200); btnOK->Width = 70; btnOK->Height = 32; btnOK->DialogResult = System::Windows::Forms::DialogResult::OK; dialog->Controls->Add(btnOK);
			Button^ btnCancel = gcnew Button(); btnCancel->Text = "Cancel"; btnCancel->Location = Point(342, 200); btnCancel->Width = 70; btnCancel->Height = 32; btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel; dialog->Controls->Add(btnCancel);

			if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				if (String::IsNullOrWhiteSpace(txtName->Text)) {
					MessageBox::Show("Category name is required.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					return;
				}
				try {
					Db::Execute("UPDATE categories SET CategoryName=@p0, Description=@p1 WHERE CategoryId=@p2", gcnew array<Object^> { txtName->Text->Trim(), txtDesc->Text->Trim(), catId });
					LoadCategories(categoriesSearchBox->Text->Trim());
					MessageBox::Show("Category updated successfully.", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
				catch (Exception^ ex) {
					MessageBox::Show("Error: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
		}

		void OnDeleteCategory(Object^ sender, EventArgs^ e) {
			if (categoriesGrid->SelectedRows->Count == 0) {
				MessageBox::Show("Please select a category to delete.", "Delete Category", MessageBoxButtons::OK, MessageBoxIcon::Information);
				return;
			}
			DataRowView^ row = (DataRowView^)categoriesGrid->SelectedRows[0]->DataBoundItem;
			int catId = Convert::ToInt32(row["CategoryId"]);
			String^ catName = Convert::ToString(row["CategoryName"]);

			// Check if category is referenced by plants
			Object^ count = Db::Scalar("SELECT COUNT(*) FROM plants WHERE CategoryId=@p0", gcnew array<Object^> { catId });
			if (Convert::ToInt32(count) > 0) {
				MessageBox::Show("Cannot delete this category because it is referenced by " + count->ToString() + " plant(s).", "Cannot Delete", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return;
			}

			if (MessageBox::Show("Delete '" + catName + "'? This action cannot be undone.", "Confirm Deletion", MessageBoxButtons::YesNo, MessageBoxIcon::Warning) != System::Windows::Forms::DialogResult::Yes) {
				return;
			}

			try {
				Db::Execute("DELETE FROM categories WHERE CategoryId=@p0", gcnew array<Object^> { catId });
				LoadCategories(categoriesSearchBox->Text->Trim());
				MessageBox::Show("Category deleted successfully.", "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error: " + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		// ============================================================================
		// SALES PAGE
		// ============================================================================

		void ShowSales() {
			PreparePage("Sales", "Record plant sales and view transaction history");

			// Sales form panel
			Panel^ formPanel = gcnew Panel();
			formPanel->BackColor = ColorWhite;
			formPanel->Dock = DockStyle::Top;
			formPanel->Height = 140;
			formPanel->Padding = System::Windows::Forms::Padding(16);
			formPanel->AutoSize = false;
			contentPanel->Controls->Add(formPanel);

			Label^ formLabel = gcnew Label();
			formLabel->Text = "Record New Sale";
			formLabel->Font = gcnew Drawing::Font("Segoe UI Semibold", 12, FontStyle::Bold);
			formLabel->ForeColor = ColorTextDark;
			formLabel->Dock = DockStyle::Top;
			formLabel->Height = 24;
			formLabel->Margin = System::Windows::Forms::Padding(0, 0, 0, 8);
			formPanel->Controls->Add(formLabel);

			// Create form inline using TableLayoutPanel
			TableLayoutPanel^ formLayout = gcnew TableLayoutPanel();
			formLayout->ColumnCount = 5;
			formLayout->RowCount = 1;
			formLayout->Dock = DockStyle::Top;
			formLayout->Height = 36;
			formLayout->Padding = System::Windows::Forms::Padding(0);
			formLayout->Margin = System::Windows::Forms::Padding(0);
			formLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 25));
			formLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 22));
			formLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 20));
			formLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 15));
			formLayout->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 18));
			formLayout->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 100));

			// Plant selector
			salesPlantCombo = gcnew ComboBox();
			salesPlantCombo->DropDownStyle = ComboBoxStyle::DropDownList;
			salesPlantCombo->Margin = System::Windows::Forms::Padding(0, 0, 4, 0);
			formLayout->Controls->Add(salesPlantCombo, 0, 0);

			// Load available plants
			try {
				DataTable^ plants = Db::Query("SELECT PlantId, PlantName, Price, StockQuantity FROM plants WHERE StockQuantity > 0 ORDER BY PlantName", gcnew array<Object^>(0));
				salesPlantCombo->DataSource = plants;
				salesPlantCombo->DisplayMember = "PlantName";
				salesPlantCombo->ValueMember = "PlantId";
			}
			catch (...) { }

			// Quantity
			salesQtyControl = gcnew NumericUpDown();
			salesQtyControl->Minimum = 1;
			salesQtyControl->Maximum = 1000;
			salesQtyControl->Value = 1;
			salesQtyControl->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			formLayout->Controls->Add(salesQtyControl, 1, 0);

			// Payment method
			salesPaymentCombo = gcnew ComboBox();
			salesPaymentCombo->DropDownStyle = ComboBoxStyle::DropDownList;
			salesPaymentCombo->Items->Add("Cash");
			salesPaymentCombo->Items->Add("Card");
			salesPaymentCombo->Items->Add("Check");
			salesPaymentCombo->Items->Add("Bank Transfer");
			salesPaymentCombo->SelectedIndex = 0;
			salesPaymentCombo->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			formLayout->Controls->Add(salesPaymentCombo, 2, 0);

			// Unit Price label
			Label^ lblPrice = gcnew Label();
			lblPrice->Font = gcnew Drawing::Font("Segoe UI", 9);
			lblPrice->ForeColor = ColorTextMuted;
			lblPrice->Text = "Price: Rs. --";
			lblPrice->TextAlign = ContentAlignment::MiddleRight;
			lblPrice->Margin = System::Windows::Forms::Padding(0, 0, 4, 0);
			formLayout->Controls->Add(lblPrice, 3, 0);

			// Record button
			Button^ btnRecord = gcnew Button();
			btnRecord->Text = "Record Sale";
			btnRecord->BackColor = ColorActionGreen;
			btnRecord->ForeColor = ColorWhite;
			btnRecord->FlatStyle = FlatStyle::Flat;
			btnRecord->FlatAppearance->BorderSize = 0;
			btnRecord->Font = gcnew Drawing::Font("Segoe UI", 10);
			btnRecord->Margin = System::Windows::Forms::Padding(4, 0, 0, 0);
			btnRecord->Click += gcnew EventHandler(this, &MainForm::OnRecordSale);
			formLayout->Controls->Add(btnRecord, 4, 0);

			formPanel->Controls->Add(formLayout);

			// Sales history label
			Label^ historyLabel = gcnew Label();
			historyLabel->Text = "Recent Sales";
			historyLabel->Font = gcnew Drawing::Font("Segoe UI Semibold", 11, FontStyle::Bold);
			historyLabel->ForeColor = ColorTextDark;
			historyLabel->AutoSize = false;
			historyLabel->Height = 36;
			historyLabel->Margin = System::Windows::Forms::Padding(0, 8, 0, 0);
			historyLabel->Dock = DockStyle::Top;
			contentPanel->Controls->Add(historyLabel);

			// Sales history grid
			salesHistoryGrid = gcnew DataGridView();
			salesHistoryGrid->BackgroundColor = ColorWhite;
			salesHistoryGrid->GridColor = ColorGridLine;
			salesHistoryGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
			salesHistoryGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
			salesHistoryGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
			salesHistoryGrid->ColumnHeadersHeight = 36;
			salesHistoryGrid->RowTemplate->Height = 28;
			salesHistoryGrid->ReadOnly = true;
			salesHistoryGrid->AllowUserToAddRows = false;
			salesHistoryGrid->RowHeadersVisible = false;
			salesHistoryGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
			salesHistoryGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
			salesHistoryGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
			salesHistoryGrid->Dock = DockStyle::Fill;
			salesHistoryGrid->Margin = System::Windows::Forms::Padding(0);
			contentPanel->Controls->Add(salesHistoryGrid);

			try {
				DataTable^ dt = Db::Query("SELECT s.SaleId, p.PlantName, s.Quantity, s.UnitPrice, s.TotalAmount, s.PaymentMethod, s.SaleDate FROM sales s JOIN plants p ON s.PlantId = p.PlantId ORDER BY s.SaleDate DESC LIMIT 50", gcnew array<Object^>(0));
				salesHistoryGrid->DataSource = dt;

				// Hide SaleId column
				if (salesHistoryGrid->Columns->Contains("SaleId")) {
					salesHistoryGrid->Columns["SaleId"]->Visible = false;
				}
			}
			catch (Exception^ ex) {
				Label^ err = gcnew Label();
				err->Text = "Error: " + ex->Message;
				err->ForeColor = ColorError;
				err->Dock = DockStyle::Top;
				contentPanel->Controls->Add(err);
			}
		}

		void OnRecordSale(Object^ sender, EventArgs^ e) {
			if (salesPlantCombo->SelectedIndex < 0) {
				MessageBox::Show("Please select a plant.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return;
			}
			if (salesQtyControl->Value < 1) {
				MessageBox::Show("Quantity must be at least 1.", "Validation", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				return;
			}

			int plantId = Convert::ToInt32(salesPlantCombo->SelectedValue);
			int qty = Convert::ToInt32(salesQtyControl->Value);
			String^ payment = salesPaymentCombo->SelectedItem->ToString();

			try {
				Decimal total = Db::RecordSale(plantId, qty, payment, userId);
				MessageBox::Show("Sale recorded successfully.\nTotal: Rs. " + total.ToString("0.00"), "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
				ShowSales(); // Refresh the page
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error recording sale:\n" + ex->Message, "Database Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		// ============================================================================
		// REPORTS PAGE
		// ============================================================================

		void ShowReports() {
			PreparePage("Reports", "View sales analytics and generate reports");

			try {
				// Create tabs for different reports
				TabControl^ tabReports = gcnew TabControl();
				tabReports->Dock = DockStyle::Fill;
				tabReports->Padding = Point(8, 8);
				contentPanel->Controls->Add(tabReports);

				// Tab 1: Sales Summary
				TabPage^ tabSalesSummary = gcnew TabPage("Sales Summary");
				tabReports->TabPages->Add(tabSalesSummary);

				DataGridView^ salesSummaryGrid = gcnew DataGridView();
				salesSummaryGrid->Dock = DockStyle::Fill;
				salesSummaryGrid->ReadOnly = true;
				salesSummaryGrid->AllowUserToAddRows = false;
				salesSummaryGrid->RowHeadersVisible = false;
				salesSummaryGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
				salesSummaryGrid->BackgroundColor = ColorWhite;
				salesSummaryGrid->GridColor = ColorGridLine;
				salesSummaryGrid->ColumnHeadersHeight = 36;
				salesSummaryGrid->RowTemplate->Height = 28;
				salesSummaryGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
				salesSummaryGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
				salesSummaryGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
				salesSummaryGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
				salesSummaryGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
				tabSalesSummary->Controls->Add(salesSummaryGrid);

				// Query: Sales by payment method
				DataTable^ salesByPayment = Db::Query(
					"SELECT PaymentMethod, COUNT(*) as TransactionCount, SUM(TotalAmount) as TotalRevenue, AVG(TotalAmount) as AvgSale "
					"FROM sales GROUP BY PaymentMethod ORDER BY TotalRevenue DESC",
					gcnew array<Object^>(0));
				salesSummaryGrid->DataSource = salesByPayment;

				// Tab 2: Inventory Status
				TabPage^ tabInventory = gcnew TabPage("Inventory Status");
				tabReports->TabPages->Add(tabInventory);

				DataGridView^ inventoryGrid = gcnew DataGridView();
				inventoryGrid->Dock = DockStyle::Fill;
				inventoryGrid->ReadOnly = true;
				inventoryGrid->AllowUserToAddRows = false;
				inventoryGrid->RowHeadersVisible = false;
				inventoryGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
				inventoryGrid->BackgroundColor = ColorWhite;
				inventoryGrid->GridColor = ColorGridLine;
				inventoryGrid->ColumnHeadersHeight = 36;
				inventoryGrid->RowTemplate->Height = 28;
				inventoryGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
				inventoryGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
				inventoryGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
				inventoryGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
				inventoryGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
				tabInventory->Controls->Add(inventoryGrid);

				// Query: Inventory with category
				DataTable^ inventory = Db::Query(
					"SELECT p.PlantName, c.CategoryName, p.Price, p.StockQuantity, (p.Price * p.StockQuantity) as InventoryValue "
					"FROM plants p LEFT JOIN categories c ON p.CategoryId = c.CategoryId ORDER BY p.StockQuantity ASC",
					gcnew array<Object^>(0));
				inventoryGrid->DataSource = inventory;

				// Tab 3: Best-Selling Plants
				TabPage^ tabBestSellers = gcnew TabPage("Best-Selling Plants");
				tabReports->TabPages->Add(tabBestSellers);

				DataGridView^ bestSellersGrid = gcnew DataGridView();
				bestSellersGrid->Dock = DockStyle::Fill;
				bestSellersGrid->ReadOnly = true;
				bestSellersGrid->AllowUserToAddRows = false;
				bestSellersGrid->RowHeadersVisible = false;
				bestSellersGrid->AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode::Fill;
				bestSellersGrid->BackgroundColor = ColorWhite;
				bestSellersGrid->GridColor = ColorGridLine;
				bestSellersGrid->ColumnHeadersHeight = 36;
				bestSellersGrid->RowTemplate->Height = 28;
				bestSellersGrid->ColumnHeadersDefaultCellStyle->BackColor = ColorOffWhite;
				bestSellersGrid->ColumnHeadersDefaultCellStyle->ForeColor = ColorTextDark;
				bestSellersGrid->ColumnHeadersDefaultCellStyle->Font = gcnew Drawing::Font("Segoe UI", 10, FontStyle::Bold);
				bestSellersGrid->DefaultCellStyle->SelectionBackColor = ColorSelectionLight;
				bestSellersGrid->DefaultCellStyle->SelectionForeColor = ColorTextDark;
				tabBestSellers->Controls->Add(bestSellersGrid);

				// Query: Best-selling plants
				DataTable^ bestSellers = Db::Query(
					"SELECT p.PlantName, COUNT(s.SaleId) as Units, SUM(s.TotalAmount) as TotalRevenue, AVG(s.TotalAmount) as AvgSalePrice "
					"FROM plants p LEFT JOIN sales s ON p.PlantId = s.PlantId GROUP BY p.PlantId, p.PlantName ORDER BY Units DESC LIMIT 20",
					gcnew array<Object^>(0));
				bestSellersGrid->DataSource = bestSellers;

				// Export buttons - use FlowLayoutPanel to avoid fixed coordinates
				Panel^ exportPanel = gcnew Panel();
				exportPanel->Dock = DockStyle::Bottom;
				exportPanel->Height = 50;
				exportPanel->BackColor = ColorOffWhite;
				exportPanel->Padding = System::Windows::Forms::Padding(16, 8, 16, 8);
				exportPanel->AutoSize = false;
				contentPanel->Controls->Add(exportPanel);

				FlowLayoutPanel^ exportFlow = gcnew FlowLayoutPanel();
				exportFlow->Dock = DockStyle::Fill;
				exportFlow->AutoSize = false;
				exportFlow->Padding = System::Windows::Forms::Padding(0);
				exportFlow->Margin = System::Windows::Forms::Padding(0);
				exportFlow->WrapContents = false;
				exportPanel->Controls->Add(exportFlow);

				Button^ btnExportSales = gcnew Button();
				btnExportSales->Text = "Export Sales Summary";
				btnExportSales->Size = Drawing::Size(160, 36);
				btnExportSales->BackColor = ColorActionGreen;
				btnExportSales->ForeColor = ColorWhite;
				btnExportSales->FlatStyle = FlatStyle::Flat;
				btnExportSales->FlatAppearance->BorderSize = 0;
				btnExportSales->Font = gcnew Drawing::Font("Segoe UI", 10);
				btnExportSales->Margin = System::Windows::Forms::Padding(0, 0, 8, 0);
				btnExportSales->Click += gcnew EventHandler(this, &MainForm::OnExportSales);
				exportFlow->Controls->Add(btnExportSales);

				Button^ btnExportInventory = gcnew Button();
				btnExportInventory->Text = "Export Inventory";
				btnExportInventory->Size = Drawing::Size(140, 36);
				btnExportInventory->BackColor = ColorActionGreen;
				btnExportInventory->ForeColor = ColorWhite;
				btnExportInventory->FlatStyle = FlatStyle::Flat;
				btnExportInventory->FlatAppearance->BorderSize = 0;
				btnExportInventory->Font = gcnew Drawing::Font("Segoe UI", 10);
				btnExportInventory->Margin = System::Windows::Forms::Padding(0, 0, 0, 0);
				btnExportInventory->Click += gcnew EventHandler(this, &MainForm::OnExportInventory);
				exportFlow->Controls->Add(btnExportInventory);
			}
			catch (Exception^ ex) {
				Label^ err = gcnew Label();
				err->Text = "Error loading reports: " + ex->Message;
				err->ForeColor = ColorError;
				err->Dock = DockStyle::Top;
				err->Padding = System::Windows::Forms::Padding(16);
				contentPanel->Controls->Add(err);
			}
		}

		void OnExportSales(Object^ sender, EventArgs^ e) {
			try {
				SaveFileDialog^ dialog = gcnew SaveFileDialog();
				dialog->FileName = "sales_" + DateTime::Now.ToString("yyyyMMdd_HHmmss") + ".csv";
				dialog->Filter = "CSV files (*.csv)|*.csv|All files (*.*)|*.*";

				if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
					IO::StreamWriter^ writer = gcnew IO::StreamWriter(dialog->FileName);

					// Write header
					writer->WriteLine("Date,Plant,Quantity,Unit Price,Total Amount,Payment Method");

					// Write data
					DataTable^ sales = Db::Query(
						"SELECT s.SaleDate, p.PlantName, s.Quantity, s.UnitPrice, s.TotalAmount, s.PaymentMethod "
						"FROM sales s JOIN plants p ON s.PlantId = p.PlantId ORDER BY s.SaleDate DESC",
						gcnew array<Object^>(0));

					for each (DataRow^ row in sales->Rows) {
						DateTime saleDate = Convert::ToDateTime(row["SaleDate"]);
						String^ date = saleDate.ToString("yyyy-MM-dd HH:mm");
						String^ plant = Convert::ToString(row["PlantName"]);
						String^ qty = Convert::ToString(row["Quantity"]);
						String^ unitPrice = Convert::ToDecimal(row["UnitPrice"]).ToString("0.00");
						String^ total = Convert::ToDecimal(row["TotalAmount"]).ToString("0.00");
						String^ payment = Convert::ToString(row["PaymentMethod"]);

						writer->WriteLine(date + "," + plant + "," + qty + "," + unitPrice + "," + total + "," + payment);
					}

					writer->Close();
					delete writer;

					MessageBox::Show("Sales report exported successfully to:\n" + dialog->FileName, "Export Complete", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error exporting sales: " + ex->Message, "Export Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		void OnExportInventory(Object^ sender, EventArgs^ e) {
			try {
				SaveFileDialog^ dialog = gcnew SaveFileDialog();
				dialog->FileName = "inventory_" + DateTime::Now.ToString("yyyyMMdd_HHmmss") + ".csv";
				dialog->Filter = "CSV files (*.csv)|*.csv|All files (*.*)|*.*";

				if (dialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
					IO::StreamWriter^ writer = gcnew IO::StreamWriter(dialog->FileName);

					// Write header
					writer->WriteLine("Plant Name,Category,Price,Stock Quantity,Inventory Value");

					// Write data
					DataTable^ inventory = Db::Query(
						"SELECT p.PlantName, c.CategoryName, p.Price, p.StockQuantity, (p.Price * p.StockQuantity) as InventoryValue "
						"FROM plants p LEFT JOIN categories c ON p.CategoryId = c.CategoryId ORDER BY p.PlantName",
						gcnew array<Object^>(0));

					for each (DataRow^ row in inventory->Rows) {
						String^ plant = Convert::ToString(row["PlantName"]);
						String^ category = (row["CategoryName"] == DBNull::Value) ? "Uncategorized" : Convert::ToString(row["CategoryName"]);
						String^ price = Convert::ToDecimal(row["Price"]).ToString("0.00");
						String^ stock = Convert::ToString(row["StockQuantity"]);
						String^ value = Convert::ToDecimal(row["InventoryValue"]).ToString("0.00");

						writer->WriteLine("\"" + plant + "\",\"" + category + "\"," + price + "," + stock + "," + value);
					}

					writer->Close();
					delete writer;

					MessageBox::Show("Inventory report exported successfully to:\n" + dialog->FileName, "Export Complete", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
			}
			catch (Exception^ ex) {
				MessageBox::Show("Error exporting inventory: " + ex->Message, "Export Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}

		// ============================================================================
		// NAVIGATION
		// ============================================================================

		void OnDashboardNav(Object^ sender, EventArgs^ e) {
			ShowDashboard();
		}

		void OnPlantsNav(Object^ sender, EventArgs^ e) {
			ShowPlants();
		}

		void OnCategoriesNav(Object^ sender, EventArgs^ e) {
			ShowCategories();
		}

		void OnSalesNav(Object^ sender, EventArgs^ e) {
			ShowSales();
		}

		void OnReportsNav(Object^ sender, EventArgs^ e) {
			ShowReports();
		}

		void OnLogout(Object^ sender, EventArgs^ e) {
			if (MessageBox::Show("Sign out of Fresh Farm?", "Sign Out", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes) {
				this->DialogResult = System::Windows::Forms::DialogResult::Retry;
				this->Close();
			}
		}
	};
}
