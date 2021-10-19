#include<bits/stdc++.h>
using namespace std;
stack<string> st;
vector<string> arr;
unordered_map<string,string> conv;
string fn;
void preparemap()
{
	conv["htpl"]="#include<stdio.h>";
	conv["main"]="int main(){";
	conv["log"]="printf()";
	conv["/log"]=";";
	conv["/main"]="}";
	conv["/htpl"]="";
}
void parsefile()
{
	ifstream fin;
 
 	string line;
    	fin.open(fn);
 	string s1;
        
    while (fin) {
         getline(fin, line);
 	int pos=0;
 	s1="";
 	if(line.length()>0)
 	{int l1=line.find_first_not_of(" \n\r\t");//Returns first index which is not ' '	
	line=line.substr(l1);//Left trimming string
        }
        while(true)
        {
        	if(!st.empty())
        	{if(st.top()!="log")
        	s1="";
        	}
        	size_t found =line.find('<',pos);
        	
        	if(found==string::npos)//If not present
        	{
        	s1+=line.substr(pos);
        	break;
        	}
        	else
        	{
        		s1+=line.substr(pos,found-pos);
        		pos=found+1;
        		size_t found =line.find('>',pos);
        		string s=line.substr(pos,found-pos);
        		if(conv.find(s)==conv.end())//If string between < and > not present in map
        		{
        			if(st.top()=="log")// If it is a part of log then print it,else give error
        			s1+="<"+s+">";
        			else
        			{
        				arr.clear();
        				cout<<"Error\n";
        				return;
        			}
        		}
        		else if(s[0]!='/')
        		{
        			if(st.size()==3)//To prevent log inside log
        			{
        				arr.clear();
        				cout<<"Error\n";
        				return;
        			}
        			
        			arr.push_back(conv[s]);
        			if(s=="log")
        			s1="";
        			st.push(s);
        		}
        		else
        		{
        			if(s.substr(1)==st.top())//If closing tag matches the topmost tag in stack
        			st.pop();
        			else
        			{
        				arr.clear();
        				cout<<"Error\n";
        				return;
        			}
        			if(s.substr(1)=="log")
        			{		
      			  	if(s1!="")
  			      	arr.push_back(s1);
        	 		}
        	 		arr.push_back(conv[s]);
        		}
        		
        		pos=found+1;
        	}
        }
        if(!st.empty())
        {
        	if(st.top()=="log")//For log spanning over multiple lines
        	{
        	if(s1!="")
        	arr.push_back(s1);
        	}
        }
    }
    // Close the file
    fin.close();
    
}
void writetofile(string outfl)
{
	ofstream fout;
 
	string line;
    fout.open(outfl);
    while (fout) {
 
       for(int i=0;i<arr.size();i++)
       {
       line=arr[i];
       if(arr[i]=="printf()")
       {
       	fout<<"printf(\"";
       	while(arr[++i]!=";")//Get the contents to be printed till we encounter ;
       	fout<<arr[i]+"\\n";
       	fout<<"\");"<<endl;
       }
       else       
        fout << line << endl;
        }
        break;
    }
 
    // Close the File
    fout.close();
 	
}
int main(int argc,char** args)
{
	fn=args[1];
   	preparemap();
   	parsefile();
 	writetofile(args[2]);
}
