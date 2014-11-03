#include <bits/stdc++.h>
using namespace std;

int main() {
	for(int i=0;i<20;i++)
	{
		for(int j=0;j<20;j++)
		{
			int v1 = rand() % 15;
			if(v1>3)v1=0;
			cout<< v1 <<" ";
		}
		cout<<endl;
	}
	return 0;
}
