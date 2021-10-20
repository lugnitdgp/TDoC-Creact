#include<bits/stdc++.h>
using namespace std;
stack<string> st;
vector<string> arr;
unordered_map<string,string> conv;
unordered_map<string,string> val;
string fn;
void preparemap()
{
	conv["htpl"]="#include<stdio.h>\n";
	conv["main"]="int main(){\n";
	conv["log"]="printf()";
	conv["/log"]=";\n";
	conv["/main"]="}";
	conv["/htpl"]="";
}

int checkdec(string s)
{
	string dtype;
	if(s.substr(0,2)=="in")
	dtype="int";
	else if(s.substr(0,2)=="ch")
	dtype="char";
	else
	return -1;//-1 returned if not declaration tag
	arr.push_back(dtype);
	int pos=2;
	while(true)
	{
		size_t found=s.find(',',pos);
		int si=pos;
		if(s[pos]==' ')
		si++;
		if(found==string::npos)
		{
			size_t found=s.find('/',pos);
			if(found==string::npos)
			return -2;
			if(found!=s.length()-1)
			return -2;
			arr.push_back(s.substr(pos,found-pos));
			size_t f1=s.find('=',pos);
			if(f1==string::npos)
			f1=found;
			val[s.substr(si,f1-si)]=dtype;
			break;
		}
		else
		{
			arr.push_back(s.substr(pos,found-pos+1));
			size_t f1=s.find('=',pos);
			if(f1==string::npos)
			f1=found;
			val[s.substr(si,f1-si)]=dtype;
			
			pos=found+1;
		}
	}
	arr.push_back(";\n");
	return 0;
}

int checkinp(string s)
{
	if(s.substr(0,4)!="take")
	return -1;//If tag not for input
	int pos=5;
	queue<string> vars;
	string sctag;
	string line="scanf(\"";
	while(true)//While loop to allow user to take multiple inputs in one tag
	{
		
		size_t found=s.find(',',pos);
		int si=pos;
		if(s[pos]==' ')
		si++;
		if(found==string::npos)//If last variable for input reached
		{
			size_t found=s.find('/',pos);
			if(found==string::npos)
			return -2;
			if(found+1!=s.length())
			return -2;
			string var=s.substr(pos,found-pos);
			vars.push(var);
			if(val.find(var)==val.end())
			return -2;
			if(val[var]=="int")
			sctag=" %d";
			else if (val[var]=="char")
			sctag=" %c";
			line+=sctag;
			break;
		}
		else
		{
			string var=s.substr(pos,found-pos);//Get variable name wjhich will store input
			vars.push(var);
			if(val.find(var)==val.end())
			return -2;
			if(val[var]=="int")
			sctag=" %d";
			else if (val[var]=="char")
			sctag=" %c";
			line+=sctag;
			
			pos=found+1;
		}
	}
	line+="\"";
	while(!vars.empty())//Add the variable names with & before for accepting input
	{
		line+=",&"+vars.front();
		vars.pop();
	}
	line+=");\n";
	arr.push_back(line);
	return 0;
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
        		int c1=checkdec(s);
        		int c2=checkinp(s);
        		
        		if(c1==-1&&c2==-1)
        		{
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
        		}
        		else if(c1==-2||c2==-2)
        		{
        			
			    	cout<<"Error in tag\n";
    				arr.clear();
    				return;
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
    if(!st.empty())
    {
    	cout<<"Error\n";
    	arr.clear();
    	return;
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
       	queue<string> q;
       	while(arr[++i]!=";\n")//Get the contents to be printed till we encounter ;
       	{
       		//fout<<arr[i]+"\\n";
       		string str="";
       		string s1="";
       		int pos=0;
       		while(true)
       		{
       			//Get variable name if any to be displayed
       			size_t found=arr[i].find("${",pos); 
       			if(found==string::npos)
       			{
       				
       				str+=arr[i].substr(pos);
       				break;
       			}
       			else
       			{
       				
       				size_t f2=arr[i].find("}",found);
       				if(f2==string::npos)
       				{
       					str+=arr[i].substr(pos);
       					break;
       				}
       				else
       				{
       					string vn=arr[i].substr(found+2,f2-found-2);
       					if(val.find(vn)!=val.end())
       					{
       						string dt=val[vn];
       						string sct;
       						if(dt=="int")
       						sct="%d";
       						else if(dt=="char")
       						sct="%c";
       						str+=arr[i].substr(pos,found-pos)+sct;
       						q.push(vn);
       					}
       					else
       					{
       						str+=arr[i].substr(pos,f2-pos+1);
       					}
       					pos=f2+1;
       				}
       			}
       		}
       		fout<<str<<"\\n";
       	}fout<<"\"";
       	while(!q.empty())
       	{
       		fout<<","<<q.front();
       		q.pop();
       	}
       	fout<<");"<<endl;
       	
       }
       else       
        fout << line;
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
