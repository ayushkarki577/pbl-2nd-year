#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <sstream>

using namespace std;

void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInteger(const string& message)
{
    int value;

    while (true)
    {
        cout << message;

        if (cin >> value)
        {
            clearInput();
            return value;
        }

        cout << "Invalid input. Please enter a valid number.\n";
        clearInput();
    }
}

string readLine(const string& message)
{
    string value;
    cout << message;
    getline(cin, value);
    return value;
}

string currentDateTime()
{
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);

    if (localTime == nullptr)
        return "Unknown";

    stringstream output;
    output << put_time(localTime, "%d-%m-%Y %H:%M:%S");

    return output.str();
}

void line()
{
    cout << "------------------------------------------------------------\n";
}

void title(const string& heading)
{
    cout << "\n";
    line();
    cout << "                 " << heading << "\n";
    line();
}

bool confirmAction(const string& message)
{
    char answer;

    cout << message << " (y/n): ";
    cin >> answer;
    clearInput();

    return answer == 'y' || answer == 'Y';
}

void pauseScreen()
{
    cout << "\nPress Enter to continue...";
    cin.get();
}

// PRODUCT CLASS

class Product
{
private:
    string serialNumber;
    string name;
    string brand;
    string category;
    string status;
    string registrationDate;
    int verificationCount;

public:
    Product()
    {
        serialNumber = "";
        name = "";
        brand = "";
        category = "";
        status = "Active";
        registrationDate = currentDateTime();
        verificationCount = 0;
    }

    Product(string serial, string productName,
            string productBrand, string productCategory,
            string productStatus = "Active", int count = 0)
    {
        serialNumber = serial;
        name = productName;
        brand = productBrand;
        category = productCategory;
        status = productStatus;
        registrationDate = currentDateTime();
        verificationCount = count;
    }

    string getSerial() const { return serialNumber; }
    string getName() const { return name; }
    string getBrand() const { return brand; }
    string getCategory() const { return category; }
    string getStatus() const { return status; }
    string getRegistrationDate() const { return registrationDate; }
    int getVerificationCount() const { return verificationCount; }

    void setName(const string& value) { name = value; }
    void setBrand(const string& value) { brand = value; }
    void setCategory(const string& value) { category = value; }
    void setStatus(const string& value) { status = value; }
    void setVerificationCount(int value) { verificationCount = value; }
    void increaseVerificationCount() { verificationCount++; }
    void setRegistrationDate(const string& value) { registrationDate = value; }

    void display() const
    {
        line();
        cout << "Serial Number      : " << serialNumber << "\n";
        cout << "Product Name       : " << name << "\n";
        cout << "Brand              : " << brand << "\n";
        cout << "Category           : " << category << "\n";
        cout << "Status             : " << status << "\n";
        cout << "Verification Count : " << verificationCount << "\n";
        cout << "Registered On      : " << registrationDate << "\n";
        line();
    }
};

// PRODUCT DATABASE - HASH TABLE WITH SEPARATE CHAINING

class ProductDatabase
{
private:
    struct HashNode
    {
        Product data;
        HashNode* next;

        HashNode(const Product& product)
        {
            data = product;
            next = nullptr;
        }
    };

    static const int TABLE_SIZE = 101;
    HashNode* table[TABLE_SIZE];
    int totalProducts;

    int hashFunction(const string& key) const
    {
        unsigned long hashValue = 0;

        for (char ch : key)
        {
            hashValue = (hashValue * 31 +
                         static_cast<unsigned char>(ch))
                        % TABLE_SIZE;
        }

        return static_cast<int>(hashValue);
    }

public:
    ProductDatabase()
    {
        totalProducts = 0;
        for (int i = 0; i < TABLE_SIZE; i++)
            table[i] = nullptr;
    }

    ~ProductDatabase() { clearDatabase(); }

    ProductDatabase(const ProductDatabase&) = delete;
    ProductDatabase& operator=(const ProductDatabase&) = delete;

    bool addProduct(const Product& product)
    {
        if (findProduct(product.getSerial()) != nullptr)
            return false;

        int index = hashFunction(product.getSerial());
        HashNode* newNode = new HashNode(product);
        newNode->next = table[index];
        table[index] = newNode;
        totalProducts++;
        return true;
    }

    Product* findProduct(const string& serial)
    {
        int index = hashFunction(serial);
        HashNode* current = table[index];

        while (current != nullptr)
        {
            if (current->data.getSerial() == serial)
                return &(current->data);

            current = current->next;
        }
        return nullptr;
    }

    const Product* findProduct(const string& serial) const
    {
        int index = hashFunction(serial);
        HashNode* current = table[index];

        while (current != nullptr)
        {
            if (current->data.getSerial() == serial)
                return &(current->data);

            current = current->next;
        }
        return nullptr;
    }

    bool deleteProduct(const string& serial, Product* deletedProduct = nullptr)
    {
        int index = hashFunction(serial);
        HashNode* current = table[index];
        HashNode* previous = nullptr;

        while (current != nullptr)
        {
            if (current->data.getSerial() == serial)
            {
                if (deletedProduct != nullptr)
                    *deletedProduct = current->data;

                if (previous == nullptr)
                    table[index] = current->next;
                else
                    previous->next = current->next;

                delete current;
                totalProducts--;
                return true;
            }

            previous = current;
            current = current->next;
        }
        return false;
    }

    bool updateProduct(const Product& updatedProduct)
    {
        Product* existing = findProduct(updatedProduct.getSerial());
        if (existing == nullptr)
            return false;

        *existing = updatedProduct;
        return true;
    }

    vector<Product> getAllProducts() const
    {
        vector<Product> products;

        for (int i = 0; i < TABLE_SIZE; i++)
        {
            HashNode* current = table[i];
            while (current != nullptr)
            {
                products.push_back(current->data);
                current = current->next;
            }
        }
        return products;
    }

    int size() const { return totalProducts; }
    bool isEmpty() const { return totalProducts == 0; }

    void clearDatabase()
    {
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            HashNode* current = table[i];
            while (current != nullptr)
            {
                HashNode* nextNode = current->next;
                delete current;
                current = nextNode;
            }
            table[i] = nullptr;
        }
        totalProducts = 0;
    }
};

// VERIFICATION HISTORY - SINGLY LINKED LIST

class VerificationHistory
{
private:
    struct HistoryNode
    {
        string serial;
        string result;
        string dateTime;
        string productName;
        HistoryNode* next;

        HistoryNode(string s, string r, string p)
        {
            serial = s;
            result = r;
            productName = p;
            dateTime = currentDateTime();
            next = nullptr;
        }
    };

    HistoryNode* head;
    int totalRecords;

public:
    VerificationHistory()
    {
        head = nullptr;
        totalRecords = 0;
    }

    ~VerificationHistory() { clear(); }

    void addRecord(const string& serial, const string& result,
                   const string& productName)
    {
        HistoryNode* newNode = new HistoryNode(serial, result, productName);
        newNode->next = head;
        head = newNode;
        totalRecords++;
    }

    void displayAll() const
    {
        if (head == nullptr)
        {
            cout << "\nNo verification history available.\n";
            return;
        }

        title("VERIFICATION HISTORY");
        HistoryNode* current = head;
        int number = 1;

        while (current != nullptr)
        {
            cout << "\nRecord Number : " << number << "\n";
            cout << "Serial Number : " << current->serial << "\n";
            cout << "Product       : " << current->productName << "\n";
            cout << "Result        : " << current->result << "\n";
            cout << "Date and Time : " << current->dateTime << "\n";
            line();
            current = current->next;
            number++;
        }
        cout << "\nTotal Records: " << totalRecords << "\n";
    }

    void searchBySerial(const string& serial) const
    {
        HistoryNode* current = head;
        bool found = false;
        int number = 1;
        title("SEARCH VERIFICATION HISTORY");

        while (current != nullptr)
        {
            if (current->serial == serial)
            {
                cout << "\nMatching Record: " << number << "\n";
                cout << "Serial Number : " << current->serial << "\n";
                cout << "Product       : " << current->productName << "\n";
                cout << "Result        : " << current->result << "\n";
                cout << "Date and Time : " << current->dateTime << "\n";
                line();
                found = true;
            }
            current = current->next;
            number++;
        }

        if (!found)
            cout << "\nNo history found for this serial number.\n";
    }

    int count() const { return totalRecords; }

    void clear()
    {
        HistoryNode* current = head;
        while (current != nullptr)
        {
            HistoryNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
        totalRecords = 0;
    }
};

// ADMIN ACTION STACK - LINKED STACK FOR UNDO

class AdminActionStack
{
private:
    struct ActionNode
    {
        string actionType;
        Product productData;
        ActionNode* next;

        ActionNode(const string& type, const Product& product)
        {
            actionType = type;
            productData = product;
            next = nullptr;
        }
    };

    ActionNode* topNode;
    int totalActions;

public:
    AdminActionStack()
    {
        topNode = nullptr;
        totalActions = 0;
    }

    ~AdminActionStack() { clear(); }

    void push(const string& actionType, const Product& product)
    {
        ActionNode* newNode = new ActionNode(actionType, product);
        newNode->next = topNode;
        topNode = newNode;
        totalActions++;
    }

    bool pop(string& actionType, Product& product)
    {
        if (topNode == nullptr)
            return false;

        ActionNode* temp = topNode;
        actionType = temp->actionType;
        product = temp->productData;
        topNode = topNode->next;
        delete temp;
        totalActions--;
        return true;
    }

    bool isEmpty() const { return topNode == nullptr; }
    int size() const { return totalActions; }

    void display() const
    {
        if (topNode == nullptr)
        {
            cout << "\nNo admin actions available to undo.\n";
            return;
        }

        title("ADMIN ACTION STACK");
        ActionNode* current = topNode;
        int number = 1;

        while (current != nullptr)
        {
            cout << number << ". " << current->actionType
                 << " | Serial: " << current->productData.getSerial()
                 << " | Product: " << current->productData.getName()
                 << "\n";
            current = current->next;
            number++;
        }
        cout << "\nThe first action will be undone first.\n";
    }

    void clear()
    {
        while (topNode != nullptr)
        {
            ActionNode* temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
        totalActions = 0;
    }
};

// OTP AUTHENTICATION SYSTEM

class OTPSystem
{
private:
    int generatedOTP;

public:
    OTPSystem() { generatedOTP = 0; }

    bool verifyOTP()
    {
        generatedOTP = 100000 + rand() % 900000;
        cout << "\nA one-time password has been generated.\n";
        cout << "DEMO OTP: " << generatedOTP << "\n";
        cout << "(In a real application, OTP would be sent "
             << "to the customer's mobile number.)\n";

        int attempts = 3;
        while (attempts > 0)
        {
            int enteredOTP = readInteger("Enter OTP: ");
            if (enteredOTP == generatedOTP)
            {
                cout << "\nOTP verified successfully.\n";
                generatedOTP = 0;
                return true;
            }

            attempts--;
            if (attempts > 0)
                cout << "Incorrect OTP. Attempts remaining: "
                     << attempts << "\n";
        }

        generatedOTP = 0;
        cout << "\nOTP verification failed. Login denied.\n";
        return false;
    }
};

class VerificationSystem;

// ABSTRACT USER CLASS

class User
{
protected:
    string userName;

public:
    User(const string& name) { userName = name; }
    virtual void showMenu(VerificationSystem& system) = 0;
    virtual ~User() {}
};

// INHERITANCE AND FUNCTION OVERRIDING

class Customer : public User
{
public:
    Customer(const string& name) : User(name) {}
    void showMenu(VerificationSystem& system) override;
};

class Admin : public User
{
public:
    Admin(const string& name) : User(name) {}
    void showMenu(VerificationSystem& system) override;
};

// MAIN VERIFICATION SYSTEM

class VerificationSystem
{
private:
    ProductDatabase database;
    VerificationHistory history;
    AdminActionStack actionStack;
    OTPSystem otp;
    string adminUsername;
    string adminPassword;
    int totalVerificationAttempts;
    int successfulVerifications;
    int unregisteredAttempts;

public:
    VerificationSystem()
    {
        adminUsername = "admin";
        adminPassword = "admin123";
        totalVerificationAttempts = 0;
        successfulVerifications = 0;
        unregisteredAttempts = 0;
        addDefaultProducts();
    }

    void addDefaultProducts()
    {
        database.addProduct(Product("SH1001", "Running Shoes", "Nike", "Footwear"));
        database.addProduct(Product("PH2001", "Smartphone", "Samsung", "Electronics"));
        database.addProduct(Product("WT3001", "Analog Watch", "Titan", "Accessories"));
        database.addProduct(Product("LP4001", "Laptop", "HP", "Electronics"));
        database.addProduct(Product("BG5001", "Travel Bag", "Wildcraft", "Bags"));
        database.addProduct(Product("HD6001", "Wireless Headphones", "boAt", "Electronics"));
        database.addProduct(Product("TB7001", "Android Tablet", "Lenovo", "Electronics"));
        database.addProduct(Product("CM8001", "Digital Camera", "Canon", "Electronics"));
    }

    bool customerLogin()
    {
        title("CUSTOMER OTP LOGIN");
        cout << "Welcome to the Product Verification System.\n";
        cout << "Verify your identity using a one-time password.\n";
        return otp.verifyOTP();
    }

    bool adminLogin()
    {
        title("ADMIN LOGIN");
        string username = readLine("Enter admin username: ");
        string password = readLine("Enter admin password: ");

        if (username == adminUsername && password == adminPassword)
        {
            cout << "\nAdmin login successful.\n";
            return true;
        }

        cout << "\nInvalid username or password.\n";
        return false;
    }

    void aboutProject() const
    {
        title("ABOUT THE PROJECT");
        cout << "PROJECT NAME:\n";
        cout << "Fake Product Detection and Verification System\n\n";
        cout << "PURPOSE:\n";
        cout << "This system helps customers check whether a "
             << "product serial number exists in the registered "
             << "product database.\n\n";
        cout << "MAIN FEATURES:\n";
        cout << "1. Customer authentication using OTP.\n";
        cout << "2. Administrator authentication.\n";
        cout << "3. Product registration and management.\n";
        cout << "4. Product authenticity verification.\n";
        cout << "5. Product status management.\n";
        cout << "6. Verification history tracking.\n";
        cout << "7. Undo functionality for admin operations.\n\n";
        cout << "DATA STRUCTURES USED:\n";
        cout << "1. Hash Table with Separate Chaining.\n";
        cout << "2. Singly Linked List.\n";
        cout << "3. Linked Stack.\n";
        cout << "4. Sorting Algorithms from the STL.\n\n";
        cout << "NOTE:\n";
        cout << "This is an educational project. Product "
             << "registration and verification are simulated "
             << "using an in-memory database.\n";
        line();
    }

    void howToVerify() const
    {
        title("HOW TO VERIFY A PRODUCT");
        cout << "Follow these steps to verify a product:\n\n";
        cout << "STEP 1:\n";
        cout << "Select Customer Login (OTP) from the main menu.\n\n";
        cout << "STEP 2:\n";
        cout << "Enter the OTP displayed on the screen.\n\n";
        cout << "STEP 3:\n";
        cout << "Select Verify Product Authenticity.\n\n";
        cout << "STEP 4:\n";
        cout << "Enter the product's unique serial number.\n\n";
        cout << "STEP 5:\n";
        cout << "The system will search the registered product "
             << "database and display the verification result.\n\n";
        cout << "POSSIBLE RESULTS:\n\n";
        cout << "REGISTERED:\n";
        cout << "The serial number exists and the product is "
             << "marked Active.\n\n";
        cout << "UNREGISTERED:\n";
        cout << "The serial number does not exist in the database.\n\n";
        cout << "BLOCKED:\n";
        cout << "The serial number exists, but the administrator "
             << "has blocked the product.\n\n";
        cout << "RECALLED:\n";
        cout << "The product has been recalled. Contact the "
             << "manufacturer or seller.\n";
        line();
    }

    void displayAllProducts() const
    {
        vector<Product> products = database.getAllProducts();
        if (products.empty())
        {
            cout << "\nNo registered products are available.\n";
            return;
        }

        title("ALL REGISTERED PRODUCTS");
        for (const Product& product : products)
            product.display();

        cout << "\nTotal Registered Products: " << products.size() << "\n";
    }

    void searchProduct() const
    {
        title("SEARCH PRODUCT");
        string serial = readLine("Enter product serial number: ");

        if (serial.empty())
        {
            cout << "\nSerial number cannot be empty.\n";
            return;
        }

        const Product* product = database.findProduct(serial);
        if (product == nullptr)
        {
            cout << "\nProduct not found in the database.\n";
            return;
        }

        cout << "\nProduct found successfully.\n";
        product->display();
    }

    void verifyProduct()
    {
        title("PRODUCT AUTHENTICITY VERIFICATION");
        string serial = readLine("Enter product serial number: ");

        if (serial.empty())
        {
            cout << "\nSerial number cannot be empty.\n";
            return;
        }

        totalVerificationAttempts++;
        Product* product = database.findProduct(serial);

        if (product == nullptr)
        {
            unregisteredAttempts++;
            cout << "\n";
            line();
            cout << "          VERIFICATION RESULT\n";
            line();
            cout << "\nSTATUS: UNREGISTERED PRODUCT\n\n";
            cout << "This serial number does not exist in "
                 << "our registered product database.\n\n";
            cout << "The authenticity of this product "
                 << "could not be verified.\n\n";
            cout << "RECOMMENDATION:\n";
            cout << "Contact the seller or manufacturer "
                 << "before purchasing or using this product.\n";
            line();

            history.addRecord(serial,
                "Unregistered - Authenticity not confirmed",
                "Unknown Product");
            return;
        }

        product->increaseVerificationCount();
        cout << "\nPRODUCT INFORMATION\n";
        cout << "Product  : " << product->getName() << "\n";
        cout << "Brand    : " << product->getBrand() << "\n";
        cout << "Category : " << product->getCategory() << "\n";
        cout << "Serial   : " << product->getSerial() << "\n\n";
        line();
        cout << "          VERIFICATION RESULT\n";
        line();

        string result;

        if (product->getStatus() == "Blocked")
        {
            result = "Blocked - Verification rejected";
            cout << "\nSTATUS: BLOCKED PRODUCT\n\n";
            cout << "This product is registered, but its "
                 << "serial number has been blocked by "
                 << "the administrator.\n\n";
            cout << "The authenticity of this product "
                 << "cannot currently be approved.\n\n";
            cout << "RECOMMENDATION:\n";
            cout << "Do not purchase or use this product. "
                 << "Contact the manufacturer or seller.\n";
        }
        else if (product->getStatus() == "Recalled")
        {
            result = "Recalled - Product should not be used";
            cout << "\nSTATUS: RECALLED PRODUCT\n\n";
            cout << "This product is registered, but it "
                 << "has been marked as recalled.\n\n";
            cout << "A recalled product may have safety, "
                 << "quality or manufacturing issues.\n\n";
            cout << "RECOMMENDATION:\n";
            cout << "Stop using this product and contact "
                 << "the manufacturer or seller immediately.\n";
        }
        else
        {
            result = "Registered - Active product record found";
            successfulVerifications++;
            cout << "\nSTATUS: REGISTERED PRODUCT\n\n";
            cout << "The serial number matches a product "
                 << "registered in our database.\n\n";
            cout << "The product is currently marked as Active.\n\n";
            cout << "The database record has been verified "
                 << "successfully.\n\n";
            cout << "NOTE:\n";
            cout << "This confirms the database registration "
                 << "and does not independently guarantee "
                 << "physical product authenticity.\n";
        }

        if (product->getVerificationCount() > 1)
        {
            cout << "\nADDITIONAL INFORMATION:\n";
            cout << "This serial number has been checked "
                 << product->getVerificationCount()
                 << " times.\n";
            cout << "If you did not perform earlier checks, "
                 << "contact the manufacturer.\n";
        }

        line();
        history.addRecord(serial, result, product->getName());
    }

    void showProductInformation() const
    {
        title("CHECK PRODUCT INFORMATION");
        string serial = readLine("Enter product serial number: ");
        const Product* product = database.findProduct(serial);

        if (product == nullptr)
        {
            cout << "\nNo product is registered with "
                 << "this serial number.\n";
            return;
        }
        product->display();
    }

    void showCustomerHistory() const { history.displayAll(); }

    void searchCustomerHistory() const
    {
        string serial = readLine("Enter serial number to search: ");
        history.searchBySerial(serial);
    }

    // ADMIN FEATURE 1: ADD NEW PRODUCT
    void adminAddProduct()
    {
        title("ADD NEW PRODUCT");
        string serial = readLine("Enter unique serial number: ");

        if (serial.empty())
        {
            cout << "\nSerial number cannot be empty.\n";
            return;
        }

        if (database.findProduct(serial) != nullptr)
        {
            cout << "\nThis serial number is already registered.\n";
            return;
        }

        string name = readLine("Enter product name: ");
        string brand = readLine("Enter brand name: ");
        string category = readLine("Enter product category: ");

        if (name.empty() || brand.empty() || category.empty())
        {
            cout << "\nProduct details cannot be empty.\n";
            return;
        }

        Product product(serial, name, brand, category);
        if (database.addProduct(product))
        {
            actionStack.push("ADD", product);
            cout << "\nProduct registered successfully.\n";
            product.display();
        }
        else
            cout << "\nUnable to register product.\n";
    }

    // ADMIN FEATURE 2: DISPLAY ALL PRODUCTS
    // Uses displayAllProducts().

    // ADMIN FEATURE 3: SEARCH PRODUCT
    // Uses searchProduct().

    // ADMIN FEATURE 4: EDIT PRODUCT DETAILS
    void adminEditProduct()
    {
        title("EDIT PRODUCT DETAILS");
        string serial = readLine("Enter product serial number: ");
        Product* product = database.findProduct(serial);

        if (product == nullptr)
        {
            cout << "\nProduct not found.\n";
            return;
        }

        cout << "\nCURRENT PRODUCT DETAILS\n";
        product->display();
        cout << "\nEnter new details.\n";
        cout << "Press Enter to keep the existing value.\n\n";

        Product oldProduct = *product;
        Product updatedProduct = *product;
        string name = readLine("New product name: ");
        string brand = readLine("New brand name: ");
        string category = readLine("New category: ");

        if (!name.empty()) updatedProduct.setName(name);
        if (!brand.empty()) updatedProduct.setBrand(brand);
        if (!category.empty()) updatedProduct.setCategory(category);

        if (oldProduct.getName() == updatedProduct.getName() &&
            oldProduct.getBrand() == updatedProduct.getBrand() &&
            oldProduct.getCategory() == updatedProduct.getCategory())
        {
            cout << "\nNo changes were made to the product.\n";
            return;
        }

        if (database.updateProduct(updatedProduct))
        {
            actionStack.push("UPDATE", oldProduct);
            cout << "\nProduct details updated successfully.\n";
            updatedProduct.display();
        }
    }

    // ADMIN FEATURE 5: UPDATE PRODUCT STATUS
    void adminUpdateStatus()
    {
        title("UPDATE PRODUCT STATUS");
        string serial = readLine("Enter product serial number: ");
        Product* product = database.findProduct(serial);

        if (product == nullptr)
        {
            cout << "\nProduct not found.\n";
            return;
        }

        cout << "\nCURRENT PRODUCT DETAILS\n";
        product->display();
        cout << "\nSELECT NEW STATUS\n";
        cout << "1. Active\n";
        cout << "2. Blocked\n";
        cout << "3. Recalled\n";

        int choice = readInteger("\nEnter your choice: ");
        string newStatus;

        switch (choice)
        {
            case 1: newStatus = "Active"; break;
            case 2: newStatus = "Blocked"; break;
            case 3: newStatus = "Recalled"; break;
            default:
                cout << "\nInvalid choice.\n";
                return;
        }

        if (product->getStatus() == newStatus)
        {
            cout << "\nProduct already has this status.\n";
            return;
        }

        Product oldProduct = *product;
        Product updatedProduct = *product;
        updatedProduct.setStatus(newStatus);

        if (database.updateProduct(updatedProduct))
        {
            actionStack.push("UPDATE", oldProduct);
            cout << "\nProduct status updated successfully.\n";
            cout << "New Status: " << newStatus << "\n";
        }
    }

    // ADMIN FEATURE 6: DELETE PRODUCT
    void adminDeleteProduct()
    {
        title("DELETE PRODUCT");
        string serial = readLine("Enter serial number to delete: ");
        Product* product = database.findProduct(serial);

        if (product == nullptr)
        {
            cout << "\nProduct not found.\n";
            return;
        }

        cout << "\nPRODUCT DETAILS\n";
        product->display();

        if (!confirmAction("Are you sure you want to delete this product?"))
        {
            cout << "\nDeletion cancelled.\n";
            return;
        }

        Product deletedProduct;
        if (database.deleteProduct(serial, &deletedProduct))
        {
            actionStack.push("DELETE", deletedProduct);
            cout << "\nProduct deleted successfully.\n";
            cout << "You can restore it using Undo Last Admin Action.\n";
        }
    }

    // ADMIN FEATURE 7: UNDO LAST ADMIN ACTION
    void undoLastAction()
    {
        title("UNDO LAST ADMIN ACTION");

        if (actionStack.isEmpty())
        {
            cout << "\nNo admin action is available to undo.\n";
            return;
        }

        actionStack.display();
        if (!confirmAction("\nUndo the most recent admin action?"))
        {
            cout << "\nUndo operation cancelled.\n";
            return;
        }

        string actionType;
        Product oldData;

        if (!actionStack.pop(actionType, oldData))
        {
            cout << "\nUndo operation failed.\n";
            return;
        }

        bool success = false;

        if (actionType == "ADD")
        {
            success = database.deleteProduct(oldData.getSerial());
            if (success)
            {
                cout << "\nUNDO SUCCESSFUL\n";
                cout << "The previously added product has been removed.\n";
            }
            else
                cout << "\nUndo failed. Product not found.\n";
        }
        else if (actionType == "DELETE")
        {
            success = database.addProduct(oldData);
            if (success)
            {
                cout << "\nUNDO SUCCESSFUL\n";
                cout << "The deleted product has been restored.\n";
            }
            else
                cout << "\nUndo failed. This serial number already exists.\n";
        }
        else if (actionType == "UPDATE")
        {
            success = database.updateProduct(oldData);
            if (success)
            {
                cout << "\nUNDO SUCCESSFUL\n";
                cout << "Previous product details have been restored.\n";
            }
            else
                cout << "\nUndo failed. Product no longer exists.\n";
        }

        if (success)
        {
            cout << "\nRestored Product Information:\n";
            oldData.display();
        }
    }

    // ADMIN FEATURE 8: SORT PRODUCTS
    void sortProducts()
    {
        title("SORT PRODUCTS");
        vector<Product> products = database.getAllProducts();

        if (products.empty())
        {
            cout << "\nNo products available to sort.\n";
            return;
        }

        cout << "SELECT SORTING METHOD\n\n";
        cout << "1. Sort by Product Name (A-Z)\n";
        cout << "2. Sort by Brand Name (A-Z)\n";
        cout << "3. Sort by Verification Count (High to Low)\n";

        int choice = readInteger("\nEnter your choice: ");

        if (choice == 1)
        {
            sort(products.begin(), products.end(),
                [](const Product& a, const Product& b)
                {
                    return a.getName() < b.getName();
                });
        }
        else if (choice == 2)
        {
            sort(products.begin(), products.end(),
                [](const Product& a, const Product& b)
                {
                    return a.getBrand() < b.getBrand();
                });
        }
        else if (choice == 3)
        {
            sort(products.begin(), products.end(),
                [](const Product& a, const Product& b)
                {
                    return a.getVerificationCount() > b.getVerificationCount();
                });
        }
        else
        {
            cout << "\nInvalid sorting choice.\n";
            return;
        }

        cout << "\nSORTED PRODUCT LIST\n";
        line();
        for (const Product& product : products)
            product.display();

        cout << "\nSorting completed successfully.\n";
    }

    void customerMenu()
    {
        int choice;
        do
        {
            title("CUSTOMER DASHBOARD");
            cout << "1. Verify Product Authenticity\n";
            cout << "2. Check Product Information\n";
            cout << "3. View Verification History\n";
            cout << "4. Search History by Serial Number\n";
            cout << "0. Logout\n";
            line();

            choice = readInteger("Enter your choice: ");
            switch (choice)
            {
                case 1: verifyProduct(); break;
                case 2: showProductInformation(); break;
                case 3: showCustomerHistory(); break;
                case 4: searchCustomerHistory(); break;
                case 0: cout << "\nCustomer logged out successfully.\n"; break;
                default: cout << "\nInvalid choice. Please try again.\n";
            }

            if (choice != 0)
                pauseScreen();

        } while (choice != 0);
    }

    void adminMenu()
    {
        int choice;
        do
        {
            title("ADMIN DASHBOARD");
            cout << "1. Add New Product\n";
            cout << "2. Display All Products\n";
            cout << "3. Search Product\n";
            cout << "4. Edit Product Details\n";
            cout << "5. Update Product Status\n";
            cout << "6. Delete Product\n";
            cout << "7. Undo Last Admin Action (Stack)\n";
            cout << "8. Sort Products\n";
            cout << "0. Logout\n";
            line();

            choice = readInteger("Enter your choice: ");
            switch (choice)
            {
                case 1: adminAddProduct(); break;
                case 2: displayAllProducts(); break;
                case 3: searchProduct(); break;
                case 4: adminEditProduct(); break;
                case 5: adminUpdateStatus(); break;
                case 6: adminDeleteProduct(); break;
                case 7: undoLastAction(); break;
                case 8: sortProducts(); break;
                case 0: cout << "\nAdmin logged out successfully.\n"; break;
                default: cout << "\nInvalid choice. Please try again.\n";
            }

            if (choice != 0)
                pauseScreen();

        } while (choice != 0);
    }

    void run()
    {
        int choice;
        do
        {
            title("FAKE PRODUCT DETECTION AND VERIFICATION SYSTEM");
            cout << "1. Customer Login (OTP)\n";
            cout << "2. Admin Login\n";
            cout << "3. About the Project\n";
            cout << "4. How to Verify a Product\n";
            cout << "5. Exit\n";
            line();

            choice = readInteger("Enter your choice: ");
            switch (choice)
            {
                case 1:
                {
                    if (customerLogin())
                    {
                        User* user = new Customer("Customer");
                        user->showMenu(*this);
                        delete user;
                    }
                    pauseScreen();
                    break;
                }
                case 2:
                {
                    if (adminLogin())
                    {
                        User* user = new Admin("Administrator");
                        user->showMenu(*this);
                        delete user;
                    }
                    pauseScreen();
                    break;
                }
                case 3:
                    aboutProject();
                    pauseScreen();
                    break;
                case 4:
                    howToVerify();
                    pauseScreen();
                    break;
                case 5:
                    cout << "\n";
                    line();
                    cout << "Thank you for using the "
                         << "Fake Product Detection and "
                         << "Verification System.\n";
                    cout << "\nApplication closed successfully.\n";
                    line();
                    break;
                default:
                    cout << "\nInvalid choice. "
                         << "Please select an option from 1 to 5.\n";
                    pauseScreen();
            }
        } while (choice != 5);
    }
};

void Customer::showMenu(VerificationSystem& system)
{
    cout << "\nWelcome, " << userName << ".\n";
    system.customerMenu();
}

void Admin::showMenu(VerificationSystem& system)
{
    cout << "\nWelcome, " << userName << ".\n";
    system.adminMenu();
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    VerificationSystem application;
    application.run();
    return 0;
}
