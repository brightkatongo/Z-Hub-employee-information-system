#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cctype>
#include <regex>

using namespace std;

// Function prototypes
string getValidInput(const string& prompt, const regex& pattern);
int getValidIntInput(const string& prompt, int min, int max);
double getValidDoubleInput(const string& prompt, double min, double max);
void clearConsole();
string departmentToString(int dept);
string statusToString(int status);
void displayHeader();

// Employee structure
struct Employee {
    string nrcNo;
    string firstName;
    string surname;
    int age;
    double basicSalary;
    string department;
    double taxAmount;
    double napsaContribution;
    double nhimaContribution;
    double netPay;
    string employmentStatus;

    // Calculate deductions and net pay
    void calculateDeductions() {
        napsaContribution = min(basicSalary * 0.05, 1221.60);
        nhimaContribution = basicSalary * 0.01;
        double taxableIncome = basicSalary - napsaContribution - nhimaContribution;
        if (taxableIncome <= 4500) {
            taxAmount = 0;
        } else if (taxableIncome <= 4800) {
            taxAmount = (taxableIncome - 4500) * 0.25;
        } else if (taxableIncome <= 6900) {
            taxAmount = 75 + (taxableIncome - 4800) * 0.30;
        } else {
            taxAmount = 705 + (taxableIncome - 6900) * 0.375;
        }
        netPay = basicSalary - (taxAmount + napsaContribution + nhimaContribution);
    }

    // Display employee information
    void display() const {
        cout << left
             << setw(15) << nrcNo
             << setw(15) << firstName
             << setw(15) << surname
             << setw(5) << age
             << setw(12) << fixed << setprecision(2) << basicSalary
             << setw(15) << department
             << setw(12) << fixed << setprecision(2) << netPay
             << setw(20) << employmentStatus << endl;
    }
};

// User structure
struct User {
    string username;
    string password;
    string role;
};

// Authenticate user
bool authenticateUser(const string& username, const string& password, User& currentUser) {
    ifstream file("users.txt");
    if (!file) {
        cerr << "Error: Unable to open users file.\n";
        return false;
    }

    string fileUsername, filePassword, fileRole;
    while (file >> fileUsername >> filePassword >> fileRole) {
        if (username == fileUsername && password == filePassword) {
            currentUser = {username, password, fileRole};
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

// Load employees from file
void loadEmployees(vector<Employee>& employees) {
    ifstream file("employees.txt");
    if (!file) {
        cerr << "Error: Unable to open employees file. Creating a new file.\n";
        return;
    }

    Employee emp;
    while (file >> emp.nrcNo >> emp.firstName >> emp.surname >> emp.age >> emp.basicSalary
           >> emp.department >>  emp.employmentStatus) {
        emp.calculateDeductions();
        employees.push_back(emp);
    }

    file.close();
}

// Save employees to file
void saveEmployees(const vector<Employee>& employees) {
    ofstream file("employees.txt");
    if (!file) {
        cerr << "Error: Unable to open employees file for writing.\n";
        return;
    }

    for (const auto& emp : employees) {
        file << emp.nrcNo << " " << emp.firstName << " " << emp.surname << " "
             << emp.age << " " << emp.basicSalary << " " << emp.department << " "
             << emp.employmentStatus << endl;
    }

    file.close();
    cout << "Employees data saved successfully.\n";
}

// Display menu based on user role
void displayMenu(const User& currentUser) {
    cout << "\n--- Z-Hub Employee Information System ---\n";
    if (currentUser.role == "Manager" || currentUser.role == "HR") {
        cout << "1. Add Employee\n"
             << "2. Delete Employee Record\n"
             << "3. Search for Employee\n"
             << "4. Edit Employee Record\n"
             << "5. Display Employees by Department\n"
             << "6. Exit\n";
    } else {
        cout << "3. Search for Employee\n"
             << "5. Display Employees by Department\n"
             << "6. Exit\n";
    }
}

// Convert department number to string
string departmentToString(int dept) {
    switch (dept) {
        case 1: return "Marketing";
        case 2: return "Development";
        case 3: return "Management";
        default: return "Unknown";
    }
}

// Convert employment status number to string
string statusToString(int status) {
    switch (status) {
        case 1: return "Permanent";
        case 2: return "Contract";
        default: return "Unknown";
    }
}

// Display header for employee information
void displayHeader() {
    cout << left
         << setw(15) << "NRC No"
         << setw(15) << "First Name"
         << setw(15) << "Surname"
         << setw(5) << "Age"
         << setw(12) << "Basic Salary"
         << setw(15) << "Department"
         << setw(12) << "Net Pay"
         << setw(20) << "Employment Status" << endl;
    cout << string(114, '-') << endl;  // Underline the header
}

// Add a new employee
void addEmployee(vector<Employee>& employees) {
    Employee newEmp;
    newEmp.nrcNo = getValidInput("Enter NRC No: ", regex("^[0-9]{6}/[0-9]{2}/[1-9]$"));
    newEmp.firstName = getValidInput("Enter First Name: ", regex("^[A-Za-z]+$"));
    newEmp.surname = getValidInput("Enter Surname: ", regex("^[A-Za-z]+$"));
    newEmp.age = getValidIntInput("Enter Age: ", 18, 100);
    newEmp.basicSalary = getValidDoubleInput("Enter Basic Salary: ", 0, 1000000);

    cout << "Select Department:\n1. Marketing\n2. Development\n3. Management\n";
    int deptChoice = getValidIntInput("Enter your choice (1-3): ", 1, 3);
    newEmp.department = departmentToString(deptChoice);

    cout << "Select Employment Status:\n1. Permanent\n2. Contract\n";
    int statusChoice = getValidIntInput("Enter your choice (1-2): ", 1, 2);
    newEmp.employmentStatus = statusToString(statusChoice);

    newEmp.calculateDeductions();
    employees.push_back(newEmp);
    cout << "Employee added successfully.\n";
}

// Delete an employee
void deleteEmployee(vector<Employee>& employees) {
    string nrcToDelete = getValidInput("Enter NRC No of employee to delete: ",
                                       regex("^[0-9]{6}/[0-9]{2}/[1-9]$"));

    auto it = remove_if(employees.begin(), employees.end(),
                        [&nrcToDelete](const Employee& emp) { return emp.nrcNo == nrcToDelete; });

    if (it != employees.end()) {
        employees.erase(it, employees.end());
        cout << "Employee deleted successfully.\n";
    } else {
        cout << "Employee not found.\n";
    }
}

// Search for an employee
void searchEmployee(const vector<Employee>& employees) {
    string nrcToSearch = getValidInput("Enter NRC No of employee to search: ",
                                       regex("^[0-9]{6}/[0-9]{2}/[1-9]$"));

    auto it = find_if(employees.begin(), employees.end(),
                      [&nrcToSearch](const Employee& emp) { return emp.nrcNo == nrcToSearch; });

    if (it != employees.end()) {
        cout << "\nEmployee Found:\n";
        displayHeader();
        it->display();
    } else {
        cout << "Employee not found.\n";
    }
}

// Edit an employee's record
void editEmployee(vector<Employee>& employees) {
    string nrcToEdit = getValidInput("Enter NRC No of employee to edit: ",
                                     regex("^[0-9]{6}/[0-9]{2}/[1-9]$"));

    auto it = find_if(employees.begin(), employees.end(),
                      [&nrcToEdit](const Employee& emp) { return emp.nrcNo == nrcToEdit; });

    if (it != employees.end()) {
        it->basicSalary = getValidDoubleInput("Enter new Basic Salary: ", 0, 1000000);
        it->calculateDeductions();
        cout << "Employee information updated.\n";
    } else {
        cout << "Employee not found.\n";
    }
}

// Display employees by department
void displayByDepartment(const vector<Employee>& employees) {
    cout << "Select Department to display:\n1. Marketing\n2. Development\n3. Management\n";
    int deptChoice = getValidIntInput("Enter your choice : ", 1, 3);
    string deptToDisplay = departmentToString(deptChoice);

    cout << "\nEmployees in " << deptToDisplay << " department:\n";
    bool found = false;
    displayHeader();
    for (const auto& emp : employees) {
        if (emp.department == deptToDisplay) {
            emp.display();
            found = true;
        }
    }
    if (!found) {
        cout << "No employees found in this department.\n";
    }
}

// Clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Get valid input based on regex pattern
string getValidInput(const string& prompt, const regex& pattern) {
    string input;
    do {
        cout << prompt;
        cin >> input;
        clearInputBuffer();
        if (!regex_match(input, pattern)) {
            cout << "Invalid input. Please try again.\n";
        }
    } while (!regex_match(input, pattern));
    return input;
}

// Get valid integer input within a range
int getValidIntInput(const string& prompt, int min, int max) {
    int input;
    do {
        cout << prompt;
        cin >> input;
        if (cin.fail() || input < min || input > max) {
            cin.clear();
            clearInputBuffer();
            cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        } else {
            clearInputBuffer();
            break;
        }
    } while (true);
    return input;
}

// Get valid double input within a range
double getValidDoubleInput(const string& prompt, double min, double max) {
    double input;
    do {
        cout << prompt;
        cin >> input;
        if (cin.fail() || input < min || input > max) {
            cin.clear();
            clearInputBuffer();
            cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        } else {
            clearInputBuffer();
            break;
        }
    } while (true);
    return input;
}

// Function to clear the console screen
void clearConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main() {
    vector<Employee> employees;
    User currentUser;

    string username, password;
    cout << "\t\t\t\t\t\t\WELCOME TO THE Z-HUB MANAGEMENT SYSTEM" << endl;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    clearConsole(); // Clear console after password entry

    if (authenticateUser(username, password, currentUser)) {
        cout << "Welcome, " << currentUser.username << " (" << currentUser.role << ")\n";
        loadEmployees(employees);

        int choice;
        do {
            displayMenu(currentUser);
            choice = getValidIntInput("Enter your choice: ", 1, 6);

            switch (choice) {
                case 1: // Add Employee
                    if (currentUser.role == "Manager" || currentUser.role == "HR") {
                        addEmployee(employees);
                    } else {
                        cout << "Access denied.\n";
                    }
                    break;
                case 2: // Delete Employee Record
                    if (currentUser.role == "Manager" || currentUser.role == "HR") {
                        deleteEmployee(employees);
                    } else {
                        cout << "Access denied.\n";
                    }
                    break;
                case 3: // Search for Employee
                    searchEmployee(employees);
                    break;
                case 4: // Edit Employee Record
                    if (currentUser.role == "Manager" || currentUser.role == "HR") {
                        editEmployee(employees);
                    } else {
                        cout << "Access denied.\n";
                    }
                    break;
                case 5: // Display Employees by Department
                    displayByDepartment(employees);
                    break;
                case 6: // Exit
                    cout << "Exiting...\n";
                    break;
            }

            if (choice != 6) {
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                clearConsole(); // Clear console after each operation
            }

        } while (choice != 6);

        saveEmployees(employees);
    } else {
        cout << "Login failed. Exiting...\n";
    }

    return 0;
}
