#include<iostream>
#include<sstream>
#include<vector>
#include<fstream>
#include<map> 
#include<cstring>
#include<cmath>
#include<set>
#include<algorithm> 
#include<cmath>
using namespace std;
int train_sen_num = 0;
int valid_sen_num = 0;
int test_sen_num=0;
int emo_num = 0; 
int vol_num = 1; 
const int max_sen = 1000;
const int max_vol = 2100;
const int max_emo = 10;
const int k=9;
vector <string> trainset[max_sen]; 
string volcab[max_vol];
string emocab[max_emo];
string train_emotag[max_sen];
string valid_emotag[max_sen];
string result_emotag[max_sen];
map<string,int> vol_base; 
map<string,int> emo_base;
double TFmatrix[max_sen][max_vol];
double TFvalid[max_sen][max_vol];
double TFtest[max_sen][max_vol];
int emo_app[max_emo]; 
struct dis_emo
{
	double dis;
	string emo;
};
void createtrainSet()
{
	ifstream fin("train_set.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	
	stringstream str;
	string sentence,vol;
	int pos;
	getline(fin,sentence);
	while(getline(fin,sentence))
	{
		pos=sentence.find(",");
		//cout<<pos<<endl;
		str<<sentence.substr(0,pos);
		while(str>>vol)
		{
			//cout<<vol<<" ";
			
			//检查是否已经出现在词汇表中
			bool flag=true;
			for(int i=1;i<vol_num;i++)
			{
				if(volcab[i]==vol)
				{
					flag=false;
					break;
				}
			} 
			if(flag)
			{ 
				volcab[vol_num++]=vol;
			}	
			 
			trainset[train_sen_num].push_back(vol);
		}
		vol=sentence.substr(pos+1);
		//cout<<vol<<" ";
		//cout<<endl;
		train_emotag[train_sen_num]=vol;
		//构建感情词汇表
		bool flag=true;
		for(int i=0;i<emo_num;i++)
		{
			if(emocab[i]==vol)
			{
				flag=false;
				break;
			}
		} 
			if(flag)
			{ 
				emocab[emo_num++]=vol;
			}
		// 
		str.clear();
		train_sen_num++;
	}
	//623 2087 cout<<train_sen_num<<" "<<vol_num; 
	fin.close();
	
	//convert_map()，为了加速查找 
	for(int i=1; i<vol_num;i++) 
    { 
        vol_base.insert(make_pair(volcab[i],i)); 
    } 
    for(int i=0; i<emo_num;i++) 
    { 
        emo_base.insert(make_pair(emocab[i],i)); 
    } 
}

void createTF()
{
	memset(TFmatrix,0,sizeof(TFmatrix));
	for(int row=0;row<train_sen_num;row++)
	{
		for(int j=0;j<trainset[row].size();j++)//每个单词进行累计 
		{
			string vol=trainset[row][j];
			int index=vol_base.find(vol)->second-1;//找到其对应词汇表中的下标 
			if(index>=0)
			TFmatrix[row][index]++;
		}
		for(int j=0;j<vol_num;j++)//计算出现概率 
		{
			TFmatrix[row][j]/=trainset[row].size(); 
		}
	}
	ofstream fout("train_TF.txt"); 
	for (int row=0;row<train_sen_num;row++)
	{
		for(int j=0;j<vol_num;j++)
		{
			fout<<TFmatrix[row][j]<< " ";
		}	
				
		fout<<endl;
	}
}
void createvalidTF()
{
	ifstream fin("validation_set.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	stringstream str;
	string sentence,vol;
	memset(TFvalid,false,sizeof(TFvalid));
	int pos;
	getline(fin,sentence);
	while(getline(fin,sentence))
	{
		pos=sentence.find(",");
		//cout<<pos<<endl;
		str<<sentence.substr(0,pos);
		int counter = 0; 
		while(str>>vol)
		{
			//cout<<vol<<" ";
			counter++;
			int index=vol_base.find(vol)->second-1;
			if(index>=0)
			TFvalid[valid_sen_num][index]++; 
		}
		for(int j=0;j<vol_num;j++)//计算出现概率 
		{
			TFvalid[valid_sen_num][j]/=counter; 
		}
			
		vol=sentence.substr(pos+1);
		valid_emotag[valid_sen_num]=vol;
		str.clear();
		valid_sen_num++;
	}
	
	ofstream fout("valid_TF.txt"); 
	for (int row=0;row<valid_sen_num;row++)
	{
		for(int j=0;j<vol_num;j++)
		{
			fout<<TFvalid[row][j]<< " ";
		}	
				
		fout<<endl;
	}
	
	//311 cout<<valid_sen_num<<endl;
}
void createtestTF()
{
	ifstream fin("test_set.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	stringstream str;
	string sentence,vol;
	memset(TFtest,false,sizeof(TFtest));
	int start,end;
	getline(fin,sentence);
	while(getline(fin,sentence))
	{
		start=sentence.find(",");
		//end=sentence.find(",",start+1);
		//cout<<pos<<endl;
		str<<sentence.substr(0,start);
		int counter = 0; 
		while(str>>vol)
		{
			//cout<<vol<<" ";
			counter++;
			int index=vol_base.find(vol)->second-1;
			if(index>=0)
			TFtest[test_sen_num][index]++; 
		}
		for(int j=0;j<vol_num;j++)//计算出现概率 
		{
			TFtest[test_sen_num][j]/=counter; 
		}
		
		str.clear();
		test_sen_num++;
	}
	for(int i=0;i<test_sen_num;i++)//计算出现概率 
	{
		for(int j=0;j<vol_num;j++)
		cout<<TFtest[i][j]<<" ";
	}
	cout<<endl;
	// cout<<test_sen_num<<endl;
}  
bool cmp(dis_emo a,dis_emo b)
{
	return a.dis<b.dis;
};
void count (string emo)
{
	int index =  emo_base.find(emo)->second;
	emo_app[index]++;
};
void KNN_Cls(int sen_num)
{
	//对于每一个validation中的句子 
	for(int senNO=0;senNO<sen_num;senNO++)
	{
		//计算距离，一个词一个词算 
		dis_emo *dis_set = new dis_emo[train_sen_num];
		for(int base=0;base<train_sen_num;base++)
		{
			double dis=0;
			for(int volNO=0;volNO<vol_num-1;volNO++)
			{	
				dis+=(TFmatrix[base][volNO]-TFvalid[senNO][volNO])*(TFmatrix[base][volNO]-TFvalid[senNO][volNO]);
			}
			dis_set[base].dis=sqrt(dis);
			dis_set[base].emo= train_emotag[base];
			
		}
		
		sort(dis_set,dis_set+train_sen_num,cmp);
		memset(emo_app,0,sizeof(emo_app));
		for(int i=0;i<k;i++)
		{
			count(dis_set[i].emo);
		}
		int likeli = emo_app[0];
		string result = emocab[0];
		for(int i=1;i<emo_num;i++)
		{
			//cout<<emocab[i]<<": "<<emo_app[i]<<"  ";
			if(emo_app[i]>likeli)
			{
				likeli=emo_app[i];
				result = emocab[i];
			}			
		}
		//cout<<endl;
		result_emotag[senNO]= result; 
	}
	
//	for(int senNO=0;senNO<sen_num;senNO++)
//	{
//		cout<<result_emotag[senNO]<<endl;
//	} 
	
	double counter = 0;
	for(int senNO=0;senNO<sen_num;senNO++)
	{
		cout<<valid_emotag[senNO]<<" "<<result_emotag[senNO]<<endl;
		if(valid_emotag[senNO]==result_emotag[senNO])
		counter++;
	} 
	cout<<"correct rate is "<<counter/sen_num;
}

int main()
{
	createtrainSet();
	createTF();
	createvalidTF();
	//createtestTF();
	KNN_Cls(valid_sen_num);
}
