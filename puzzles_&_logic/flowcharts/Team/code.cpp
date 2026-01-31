#include <iostream>

using namespace std;

int main(){
    int n, count;
    cin >> n;
    count = 0;
    while(n--){
        int a, b, c;
        cin >> a >> b >> c;
        if(a+b+c >= 2){
            count++;
        }
    }
    cout << count << endl;
    return 0; 
}