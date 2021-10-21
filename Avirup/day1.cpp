#include<bits/stdc++.h>
using namespace std;
stack<string> st;
vector<string> arr;
unordered_map<string,string> conv;
unordered_map<string,string> cond;

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
	cond["if"]="if";
	cond["elif"]="else if";
	cond["else"]="else";
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
int checkarith(string s)
{
	if(s[0]!='%')
	return -1;
	if(s[s.length()-1]!='%')
	return -2;
	arr.push_back(s.substr(1,s.length()-2)+";\n");
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
        
 	//cout<<"B "<<line<<endl;
        while(true)
        {
        	//cout<<"c "<<line.substr(pos)<<line.substr(pos).length()<<" "<<st.size()<<endl;
        	
        	if(!st.empty())
        	{if(st.top()!="log")
        	s1="";
        	}
        	size_t fen=line.find_first_not_of(" \n\r\t",pos);
        	//size_t fend=line.find("?>",pos);
        	if(fen!=string::npos)
        	{
        		size_t fq=line.find_first_not_of("?",fen);
        		if(line[fq]=='>')
        		{
        		string t=st.top();
        		if(t[1]=='?'&& t.length()==fq-fen+1)
        		{
        		st.pop();
        		//cout<<"Y"<<endl;
        		arr.push_back("}\n");
        		pos=fq+1;
        		continue;
        		}
        		else if(line[fen]=='?')
        		{
        				arr.clear();
        				cout<<"Error\n";
        				return;
        		}
        		}
        		
        	}
        	size_t found =line.find('<',pos);
        	if(found==string::npos)//If not present
        	{
        	s1+=line.substr(pos);
        	break;
        	}
        	else
        	{
        		if(line[found+1]!='?')
        		{
        	
        		s1+=line.substr(pos,found-pos);
        		pos=found+1;
        		size_t found =line.find('>',pos);
        		string s=line.substr(pos,found-pos);
        		//cout<<"H1"<<endl;
        		int c1=checkdec(s);
        		//cout<<"H2"<<endl;
        		int c2=checkinp(s);
        		//cout<<"H3"<<endl;
        		int c3=checkarith(s);
        		//cout<<"H4"<<endl;
        		if(c1==-1&&c2==-1&&c3==-1)
        		{
        		if(conv.find(s)==conv.end())//If string between < and > not present in map
        		{
        			if(st.top()=="log")// If it is a part of log then print it,else give error
        			s1+="<"+s+">";
        			else
        			{
        				arr.clear();
        				cout<<"ErrorA\n";
        				return;
        			}
        		}
        		else if(s[0]!='/')
        		{
        			if(!st.empty())
        			{
        			if(st.top()=="log")//To prevent log inside log
        			{
        				arr.clear();
        				cout<<"ErrorB\n";
        				return;
        			}
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
        				cout<<"ErrorC"<<st.top()<<endl;
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
        		else if(c1==-2||c2==-2||c3==-2)
        		{
        			
			    	cout<<"Error in tag\n";
    				arr.clear();
    				return;
        		}
        		pos=found+1;
        	}
        	
        	else
        	{
        			size_t fq=line.find_first_not_of('?',found+1);
        			st.push(line.substr(found,fq-found));
        			
        			size_t f1=line.find_first_not_of(" \n\r\t",fq);
        			
        			while(f1==string::npos)
        			{
        			cout<<"X"<<endl;
        			getline(fin,line);
        			
        			 f1=line.find_first_not_of(" \n\r\t");
        			}
        			//if(line.length()>0)
 				//{int l1=line.find_first_not_of(" ",f1);//Returns first index which is not ' '	
				line=line.substr(f1);//Left trimming string
        			//}	
        			//cout<<line<<endl;
        			f1=0;
        			if(line.substr(f1,2)!="if"&&line.substr(f1,4)!="else"&&line.substr(f1,4)!="elif")
        			{
        				cout<<"If not after <?"<<endl;
        				arr.clear();
        				return;
        			}
        			else
        			{
        				string c;
        				size_t f2;
        				if(line.substr(f1,2)=="if")
        				{
        					c=line.substr(f1,2);
        					f2=f1+2;
        				}
        				else if(line.substr(f1,4)=="elif")
        				{
        					c=line.substr(f1,4);
        					f2=f1+4;
        				}
        				else if(line.substr(f1,4)=="else")
        				{
        					c=line.substr(f1,4);
        					f2=f1+4;
        				}
        				string tag=cond[c];
        				if(c!="else")//If and elif will be followed by paranthesis while else will not
        				{
        				stack<string> op;
        				op.push("(");size_t fs=f2;f2++;
        				while(!op.empty())
        				{
        					if(line[f2]=='(')
        					op.push("(");
        					if(line[f2]==')')
        					op.pop();
        					f2++;
        				}
        				arr.push_back(tag+line.substr(fs,f2-fs)+"{\n");
        				}
        				else
        				{
        				arr.push_back(tag+"{\n");
        				}
        				pos=f2;
        				
        			}
        			
        		
        	}
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
