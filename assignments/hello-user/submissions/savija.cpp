#include <iostream>
#include <string>
using namespace std;

int main() {

    // Personal Information
    string fullName;
    short age;
    char gender;
    long studentID;

    // Academic Information
    int enrollmentYear;
    int daysAttended;
    float gpa;

    // Physical Measurements
    double height;
    float weight;

    // Contact Information
    long long phoneNumber;
    int postalCode;

    // Welcome Message
    cout << "========================================\n";
    cout << "    STUDENT INFORMATION SYSTEM\n";
    cout << "========================================\n\n";

    cout << "Please enter the following information:\n\n";

    // Input Section
    cout << "--- Personal Information ---\n";
    cout << "Full Name: ";
    getline(cin, fullName);

    cout << "Age: ";
    cin >> age;

    cout << "Gender (M/F/O): ";
    cin >> gender;

    cout << "Student ID: ";
    cin >> studentID;

    cout << "\n--- Academic Information ---\n";
    cout << "Year of Enrollment: ";
    cin >> enrollmentYear;

    cout << "Total Days Attended: ";
    cin >> daysAttended;

    cout << "GPA: ";
    cin >> gpa;

    cout << "\n--- Physical Measurements ---\n";
    cout << "Height (in meters): ";
    cin >> height;

    cout << "Weight (in kg): ";
    cin >> weight;

    cout << "\n--- Contact Information ---\n";
    cout << "Phone Number: ";
    cin >> phoneNumber;

    cout << "Postal Code: ";
    cin >> postalCode;

    // Output Report
    cout << "\n========================================\n";
    cout << "       STUDENT INFORMATION REPORT\n";
    cout << "========================================\n\n";

    cout << "Personal Information:\n";
    cout << "  Name: " << fullName << endl;
    cout << "  Age: " << age << " years old\n";
    cout << "  Gender: " << gender << endl;
    cout << "  Student ID: " << studentID << endl;

    cout << "\nAcademic Information:\n";
    cout << "  Enrollment Year: " << enrollmentYear << endl;
    cout << "  Days Attended: " << daysAttended << " days\n";
    cout << "  GPA: " << gpa << endl;

    cout << "\nPhysical Measurements:\n";
    cout << "  Height: " << height << " meters\n";
    cout << "  Weight: " << weight << " kg\n";

    cout << "\nContact Information:\n";
    cout << "  Phone: " << phoneNumber << endl;
    cout << "  Postal Code: " << postalCode << endl;

    cout << "\n========================================\n";
    cout << "Thank you for using the Student Information System!\n";
    cout << "========================================\n";

    return 0;
}