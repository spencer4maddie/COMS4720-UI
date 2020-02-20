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
int emo_num = 0; 
int vol_num = 1; 
const int max_sen = 1000;
const int max_vol = 2100;
const int max_emo = 10;
vector <string> trainset[max_sen]; 
string volcab[max_vol];
string emocab[max_emo];
string train_emotag[max_sen];
string valid_emotag[max_sen];
string result_emotag[max_sen];
map<string,int> vol_base; 
map<string,int> emo_base;
double POW[max_emo][max_vol];
double POE[max_emo];
int emo_app[max_emo];
int OHvalid[max_sen][max_vol];
struct pos_emo
{
	double pos;
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
			
			//����Ƿ��Ѿ������ڴʻ����
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
		//��������ʻ��
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
	
	//convert_map()��Ϊ�˼��ٲ��� 
	for(int i=1; i<vol_num;i++) 
    { 
        vol_base.insert(make_pair(volcab[i],i)); 
    } 
    for(int i=0; i<emo_num;i++) 
    { 
        emo_base.insert(make_pair(emocab[i],i)); 
    } 
}

void createPOW()
{
	for(int emo_type=0;emo_type<emo_num;emo_type++)
		for(int index=0;index<vol_num-1;index++)
			POW[emo_type][index]=1;
	
	for(int i=0;i<emo_num;i++)
		emo_app[i]=vol_num-1;
	memset(POE,false,sizeof(POE));
	
	string vol;

	for(int row=0;row<train_sen_num;row++)//�����ı� 
	{
		int emo_type = emo_base.find(train_emotag[row])->second;
		for(int j=0;j<trainset[row].size();j++)//�����ı��е�ÿ������ 
		{
			vol=trainset[row][j];
			int index=vol_base.find(vol)->second-1;//�ҵ����ڴʻ���ж�Ӧ���±� 
			POW[emo_type][index]++;// ���Ӹ�����£��õ��ʵĳ������� 
			emo_app[emo_type]++;// ���Ӹ�����µĵ������� 
		}
		POE[emo_type]++; //����ͳ��������� 
	}
	
	for(int emo_type=0;emo_type<emo_num;emo_type++)
	{
		for(int index=0;index<vol_num-1;index++)
		{
			POW[emo_type][index]/=emo_app[emo_type];//���㵥����ĳһ����³��ֵĸ��� 
		}
		
		POE[emo_type]/=train_sen_num; //���������е�������� 
	}
//	for(int word=0;word<vol_num-1;word++)
//	{
//		for(int emo_type=0;emo_type<emo_num;emo_type++)
//		{
//			fout<<POW[emo_type][word]<<" ";
//		}
//		fout<<endl;
//	}
}

void createvalidOH()
{
	ifstream fin("test_set.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	stringstream str;
	string sentence,vol;
	memset(OHvalid,false,sizeof(OHvalid));
	int pos,end;
	getline(fin,sentence);
	while(getline(fin,sentence))
	{
		pos=sentence.find(",");
		end=sentence.find(",",pos+1);
		//cout<<pos<<endl;
		str<<sentence.substr(pos+1,end-pos-1);
		//str<<sentence.substr(pos,end);
		while(str>>vol)
		{
			//cout<<vol<<" ";
			int index=vol_base.find(vol)->second-1;
			if(index>=0)
			OHvalid[valid_sen_num][index]++; 
		}

		//vol=sentence.substr(pos+1);
		//valid_emotag[valid_sen_num]=vol;
		str.clear();
		valid_sen_num++;
	}
	
	//311 cout<<valid_sen_num<<endl;
} 
bool cmp(pos_emo a,pos_emo b)
{
	return a.pos>b.pos;
};

void KNN_Cls(int sen_num)
{
	//����ÿһ��validation�еľ��� 
	
	//��������ԣ�һ����һ�����㣬p(����|ĳ�����)***p(���) 
	for(int senNO=0;senNO<sen_num;senNO++)
	{
		pos_emo *pos_set = new pos_emo[emo_num];
		for(int i=0;i<emo_num;i++)
		{
			pos_set[i].pos=POE[i];
			pos_set[i].emo=emocab[i];
		}
		
		for(int index=0;index<vol_num-1;index++)
		{
			if(OHvalid[senNO][index]>0)
			{
				for(int i=0;i<emo_num;i++)
				{
					pos_set[i].pos*=pow(POW[i][index],OHvalid[senNO][index]);
				}
			}
		}	
		for(int i=0;i<emo_num;i++)
			cout<<pos_set[i].emo<<": "<<pos_set[i].pos<<endl;		
		sort(pos_set,pos_set+emo_num,cmp);
		
		result_emotag[senNO]=pos_set[0].emo;
		//cout<<result_emotag[senNO]<<endl;
	}
		
	double counter = 0;
	//ofstream fout("result_NB_cls.txt");
	for(int senNO=0;senNO<sen_num;senNO++)
	{
		cout<<result_emotag[senNO]<<endl;
		//cout<<valid_emotag[senNO]<<" "<<result_emotag[senNO]<<endl;
		//if(valid_emotag[senNO]==result_emotag[senNO])
		//counter++;
	} 
	//cout<<"correct rate is "<<counter/sen_num;
}

int main()
{
	createtrainSet();
	createPOW();
	createvalidOH();
	KNN_Cls(valid_sen_num);
}
