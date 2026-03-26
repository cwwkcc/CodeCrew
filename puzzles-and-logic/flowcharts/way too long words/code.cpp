#include <iostream>

using namespace std;

int main(){
     
    int n;
    
    cin >> n;

    while(n--){
        string s;
        cin >> s;
        int length =s.length();
        if(length < 11){
            cout << s << "\n";
        }
        else{
            string temp = "";
            temp += s[0];
            temp += to_string(length - 2);
            temp += s[length- 1];
            cout << temp << "\n";
        }
    }
    return 0;
}