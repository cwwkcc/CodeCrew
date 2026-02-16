#include <iostream>
#include <string>
using namespace std;

int main() {
    
    string fullName;
    short age;
    char gender;
    long studentID;
    int enrollmentYear;
    int daysAttended;
    float gpa;
    double height;
    float weight;   
    long long phoneNumber;
    int postalCode;

    cout<< "========================================" <<endl;
    cout<< "|      STUDENT INFORMATION SYSTEM      |" <<endl;
    cout<< "========================================" <<endl;
    cout<< "PLease Enter the following Information" <<endl;
    cout<< "" <<endl;

    cout<< "Full name :  ";
    getline(cin, fullName);
    cout<< "Gender (M/F) :  ";
    cin>> gender;
    cout<< "Student ID No. :    ";
    cin>> studentID;

    cout<<"Year of Enrollment :    ";
    cin>> enrollmentYear;
    cout<<"Days Attended :    ";
    cin>> daysAttended;
    cout<<"GPA :    ";
    cin>> gpa;

    cout<< "Height (m) :    ";
    cin>> height;
    cout<< "Weight (kg) :    ";
    cin>> weight;

    cout<< "Phone Number :    ";
    cin>> phoneNumber;
    cout<< "Postal Code :    ";
    cin>> postalCode;

    cout<< "" <<endl;
    cout<< "" <<endl;
    cout<< "" <<endl;

    cout<< "========================================" <<endl;
    cout<< "|             STUDENT REPORT           |" <<endl;
    cout<< "========================================" <<endl;
    cout<< "" <<endl;
    
    cout<< "----Personal Information----" <<endl;
    cout<< "Full name : "               + fullName <<endl;
    cout<< "Gender : ";                 cout<< gender <<endl;
    cout<< "Student ID No. : ";         cout<< studentID <<endl;
    cout<< "" <<endl;
    
    cout<< "----Academic Information----" <<endl;
    cout<<"Year of Enrollment : ";      cout<< enrollmentYear <<endl;
    cout<<"Days Attended : ";           cout<< daysAttended <<endl;
    cout<<"GPA : ";                     cout<< gpa <<endl;
    cout<<"" <<endl;
    
    cout<<"----Physical Information----" <<endl;
    cout<< "Height in meters : ";       cout<< height <<endl;
    cout<< "Weight in kilograms : ";    cout<< weight <<endl;
    cout<<"" <<endl;

    cout<<"----Contact Information----" <<endl;
    cout<< "Phone Number : ";           cout<< phoneNumber <<endl;
    cout<< "Postal Code : ";            cout<< postalCode <<endl;
    cout<<""<<endl;
    cout<<"______________________________________________________"<<endl;
    cout<<"|***Thankyou for using STUDENT INFORMATION SYSYTEM***|"<<endl;
    cout<<"|                    Written in C++                  |"<<endl;
    cout<<"|                  By: Vihas Aadithya                |"<<endl;
    cout<<"------------------------------------------------------"<<endl;
return 0;
}