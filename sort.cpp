#include<bits/stdc++.h>
using namespace std; 
/*============================================
COMPARE FUNCTION COMAPRE 2ND COLUMN ELEMENTS
=============================================*/
bool compare_col(const vector<int>& v1,const vector<int>& v2) 
{ 
    return v1[1] < v2[1]; 
} 
/*==========================================================
FUNCTION TO SORT 2D VECTOR ON THE BASIS OF PARTICULAR COLUMN
===========================================================*/
void sorting(vector<vector<int>>A)
{
    //n contain no.of rows
    int n=A.size();
    //m contain no. of columns
    int m=A[0].size();
    cout<<"Sorting 2D vector on the basis of 2nd column\n";
    // Displaying the 2D vector after sorting 
    cout << "2D vector before sorting :"<<endl; 
    for (int i=0; i<n; i++) 
    { 
        for (int j=0; j<m ;j++)
        {
            cout << A[i][j] << " "; 
        }cout<<endl;
    } 
    // Using sort() function to sort.
    //compare_col parameter used to compare col elements.
    //And sort accordingly.
    sort(A.begin(), A.end(),compare_col); 
    // Displaying the 2D vector after sorting 
    cout << "2D vector after sorting :"<<endl; 
    for (int i=0; i<n; i++) 
    { 
        for (int j=0; j<m ;j++) 
            {
                cout << A[i][j] << " "; 
            }cout << endl; 
    } 
}
/*======================================
   MAIN FUNCTION
=======================================*/
int main() 
{ 
    // Initializing 2D vector
    vector< vector<int> > vect{{6, 4, 1}, 
                               {9, 8, 2}, 
                               {1, 2, 9}};
    //Calling sorting function
    sorting(vect); 
    return 0; 
} 