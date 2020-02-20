#include<iostream>
#include<vector>
#include<sstream>
#include<cstring>
#include<fstream>
#include<set>
#include<cmath> 
using namespace std;
const int max_ent_num=1000;
int ent_num=0;
int train_ent_num=0;
int valid_ent_num=0; 
int test_ent_num=0;
int attr_num;
double set_attr[max_ent_num][15];
double test_attr[max_ent_num][15];
double train_attr[max_ent_num][15];
double valid_attr[max_ent_num][15];
double train_tag[max_ent_num];
double test_tag[max_ent_num];
double valid_tag[max_ent_num];
double set_tag[max_ent_num];
struct value
{
	int val=-1;
	int cnt[2]={0,0};
	int sum=0;
};
int attr_value_num[15];
int visited[50];
double entropy[15];
double conentropy[15];
double G[15];
double GR[15];
double Gi[15];
struct subset
{
	vector<int> ent_list;
	vector<int> attr_list;
};
void getTrain()
{
	ifstream fin("train.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	stringstream str;
	string sentence;
	double x;
	int start=0,end; 
	while(getline(fin,sentence))
	{
		attr_num=0;
		start=0;
		while(sentence.find(",",start)!=-1)
		{	
			end=sentence.find(",",start);
			str<<sentence.substr(start,end-start);
			str>>x; 
			
			set_attr[ent_num][attr_num++]=x;
			start=end+1;
			str.clear();		
		}
		str<<sentence.substr(start);
		str>>x;
		set_tag[ent_num++]=x;
		str.clear();
	}
	//cout<<ent_num<<" "<<attr_num<<endl;
	
	for(int i=0;i<attr_num;i++)
	{
		for(int k=0;k<50;k++)
			visited[k]=-1;
		attr_value_num[i]=0;
		for(int j=0;j<ent_num;j++)
		{
			if(visited[(int)set_attr[j][i]]==-1)
			{
				attr_value_num[i]++;
				visited[(int)set_attr[j][i]]=1;	
			}
			
			//cout<<value_domain[i][j].cnt[0]<<": "<<value_domain[i][j].cnt[1]<<endl;
		} 
		cout<<attr_value_num[i]<<endl;
	}
	//cout<<D.ent_list.size()<<" "<<D.attr_list.size()<<endl;		
} 
void getValid()
{
	train_ent_num=0;
	valid_ent_num=0;
	for(int i=0;i<ent_num;i++)
	{
		if(i>=100&&i<=550)
		{
			for(int j=0;j<attr_num;j++)
			{
				train_attr[train_ent_num][j]=set_attr[i][j];	
				
			}
			train_tag[train_ent_num]=set_tag[i];
			train_ent_num++; 
		}
		else
		{
			for(int j=0;j<attr_num;j++)
			{
				valid_attr[valid_ent_num][j]=set_attr[i][j];	
			}
			valid_tag[valid_ent_num]=set_tag[i];
			valid_ent_num++;
		}		
	}
}
void getTest()
{
	ifstream fin("test.csv");
	if(!fin)
	{
		cout<<"Erro while open file"<<endl;
		return;
	}
	stringstream str;
	string sentence;
	double x;
	int start=0,end; 
	while(getline(fin,sentence))
	{
		attr_num=0;
		start=0;
		while(sentence.find(",",start)!=-1)
		{	
			end=sentence.find(",",start);
			str<<sentence.substr(start,end-start);
			str>>x; 
			//cout<<x<<" ";
			test_attr[test_ent_num][attr_num++]=x;
			start=end+1;
			str.clear();		
		}
		test_ent_num++;
		//cout<<endl;
	}
	//cout<<ent_num<<" "<<attr_num<<endl;
	/*for(int i=0;i<test_ent_num;i++)
	{
		for(int j=0;j<attr_num;j++)
			cout<<test_attr[i][j]<<" ";
		cout<<endl;
	}*/
} 
void build_domain(subset D,vector<value> value_domain[15])
{
	//cout<<"现在处理的数据集有 "<<D.attr_list.size()<<"个属性以及"<<D.ent_list.size()<<"个句子"<<endl; 
	for(int i=0;i<D.attr_list.size();i++)
	{
		for(int k=0;k<50;k++)
			visited[k]=-1;
		int cnt=0;
		int a=D.attr_list[i]; 
		//cout<<"处理的是第"<<a<<"个属性"<<endl; 
		for(int j=0;j<D.ent_list.size();j++)
		{
			int val=train_attr[D.ent_list[j]][a];
			//cout<<" 这个句子在这个属性下的取值为"<<val<<endl;
			if(visited[val]==-1)
			{
				value v;
				v.val=val;
				v.sum=1;
				if(train_tag[D.ent_list[j]]==-1)
				{
					v.cnt[0]=1;
					v.cnt[1]=0;
				}	
				else
				{
					v.cnt[1]=1;
					v.cnt[0]=0;
				}
				//cout<<"这是一个新取值"<<endl;
				//cout<<value_domain[i].size()<<endl; 
				value_domain[i].push_back(v);
				visited[val]=cnt++;		
			}
			else
			{
				if(train_tag[D.ent_list[j]]==-1)
					value_domain[i][visited[val]].cnt[0]++;
				else
					value_domain[i][visited[val]].cnt[1]++;
				value_domain[i][visited[val]].sum++;
			}
		}
	}
	
	for(int k=0;k<2;k++)
	{	
		value v;
		if(k==0) v.val=-1;
		else v.val=1;
		v.sum=0;
		value_domain[D.attr_list.size()].push_back(v);
	}
	for(int j=0;j<D.ent_list.size();j++)
	{	
		if(train_tag[D.ent_list[j]]==1)
			value_domain[D.attr_list.size()][1].sum++;
		else if(train_tag[D.ent_list[j]]==-1)
			value_domain[D.attr_list.size()][0].sum++;
	}
	/*for(int i=0;i<D.attr_list.size();i++)
	{
		for(int j=0;j<value_domain[i].size();j++)
			cout<<value_domain[i][j].cnt[0]<<": "<<value_domain[i][j].cnt[1]<<endl;
		cout<<endl;
	}*/
	//cout<<"value_domain建好了"<<endl;
}

void get_entro(vector<value> value_domain[15],subset D)
{
	//cout<<"商啊"<<endl;	
	for(int i=0;i<=D.attr_list.size();i++)
	{
		entropy[i]=0;
		for(int j=0;j<value_domain[i].size();j++)
		{
			double p=(double)value_domain[i][j].sum/(double)D.ent_list.size();
			double logp;
			if(p==0)
				logp=0;
			else
				logp=log(p);
			entropy[i]+=-( p*logp);
		}
		
		//cout<<entropy[i]<<endl;
	}
	//cout<<entropy[D.attr_list.size()]<<endl; 
}
int ID_3(vector<value> value_domain[15],subset D)
{
	//cout<<"ID_3"<<endl;
	double max=-1;
	int mark=-1; 
	for(int i=0;i<D.attr_list.size();i++)
	{
		conentropy[i]=0;
		for(int j=0;j<value_domain[i].size();j++)
		{
			double p=(double)value_domain[i][j].sum/(double)D.ent_list.size();
			double p0=(double)value_domain[i][j].cnt[0]/(double)value_domain[i][j].sum;
			double p1=(double)value_domain[i][j].cnt[1]/(double)value_domain[i][j].sum;
			//cout<<" "<<p0<<" "<<p1<<endl;
			double log0,log1;
			if(p0!=0)
				log0=log(p0);
			else
				log0=0;
			if(p1!=0)
				log1=log(p1);
			else 
				log1=0;
			conentropy[i]+=p*( -p0*log0-p1*log1 );
		}
		
		G[i]=entropy[D.attr_list.size()]-conentropy[i];
		//cout<<G[i]<<endl;
		if(G[i]>max)
		{
			max=G[i];
			mark=i;
		}
	}//cout<<endl;
	return mark;	
}
int C4_5(subset D)
{ 
	//cout<<"C4_5"<<endl;
	double max=-1;
	int mark=-1;
	for(int i=0;i<D.attr_list.size();i++)
	{
		if(entropy[i]==0)
			entropy[i]=0.000001; 
		GR[i]=G[i]/entropy[i];
		//cout<<GR[i]<<endl;
		if(max<GR[i])
		{
			max=GR[i];
			mark=i;
		}
	}
	//cout<<endl;
	return mark;
}
int GINI(vector<value> value_domain[15],subset D)
{
	//cout<<"GINI"<<endl;
	double min=1;
	int mark=-1;
	for(int i=0;i<D.attr_list.size();i++)
	{
		Gi[i]=0;
		for(int j=0;j<value_domain[i].size();j++)
		{
			double p=(double)value_domain[i][j].sum/(double)D.ent_list.size();
			double p0=(double)value_domain[i][j].cnt[0]/(double)value_domain[i][j].sum;
			double p1=(double)value_domain[i][j].cnt[1]/(double)value_domain[i][j].sum;
			//cout<<" "<<p0<<" "<<p1<<endl;
			Gi[i]+=p*(1-p0*p0-p1*p1 );
		}
		//cout<<Gi[i]<<endl;
		if(Gi[i]<min)
		{
			min=Gi[i];
			mark=i;
		}
	}
	//cout<<endl;
	return mark;
}
struct node
{
	int attr;
	double value=-1;
	node **child=NULL;
	int result=0;
	int child_num=0;
};

vector<subset> divide_data(subset D,int a,vector<value> value_domain[15])
{
	vector<subset> temp;
	for(int i=0;i<value_domain[a].size();i++)
	{
		subset sub;
		for(int k=0;k<D.attr_list.size();k++)
		{
			if(k!=a)
			sub.attr_list.push_back(D.attr_list[k]);
		}
		for(int k=0;k<D.ent_list.size();k++)
		{
			if(train_attr[D.ent_list[k]][D.attr_list[a]]==value_domain[a][i].val)
			sub.ent_list.push_back(D.ent_list[k]);
		}
		//cout<<"第"<<D.attr_list[a]<<"个属性为"<<value_domain[a][i].val<<"有" <<sub.ent_list.size()<<" 个"<<endl; 
		temp.push_back(sub); 
	}
	//cout<<endl; 
	return temp; 
}
int choose_attr(vector<value> value_domain[15],subset D,string A)
{
	get_entro(value_domain,D);
	if(A=="USE_ID3")
	{
		return ID_3(value_domain,D);
	}
	else if(A=="USE_C4_5")
	{
		ID_3(value_domain,D);
		return C4_5(D);
	}
	else if(A=="USE_CART")
	{
		return GINI(value_domain,D);
	}
}
bool meet_with_bound(vector<value> value_domain[15],subset D,node *parent)
{
	/* three conditions */	
	get_entro(value_domain,D);
	int result;
	if(value_domain[D.attr_list.size()][1].sum>value_domain[D.attr_list.size()][0].sum)
		result=1;
	else
		result=-1;
	if(entropy[D.attr_list.size()]==0)
	{
		parent->result=result;
		return true;
	}
	
	if(D.attr_list.size()==0)
	{
		parent->result=result;
		return true;
	}
				
	return false;
}
//ofstream fout("Reasut1.txt");
void buildtree(subset D,node *parent,vector<value> value_domain[15])
{
	if(meet_with_bound(value_domain,D,parent))
	{
		return;
	}
	string A="USE_ID3";//"USE_C4_5"  "USE_CART" "USE_ID3"
	int a=choose_attr(value_domain,D,A);
	//cout<<A<<endl;
	//fout<<D.attr_list[a]<<endl;
	if(value_domain[D.attr_list.size()][1].sum>value_domain[D.attr_list.size()][0].sum)
		parent->result=2;
	else
		parent->result=-2;
	/**/
	int n=value_domain[a].size(); 
	if(n<attr_value_num[D.attr_list[a]])
	{
		parent->child=(node**)malloc( (n+1)*sizeof (node*));
		parent->child_num=n+1;
		parent->child[n]=new node;
		parent->child[n]->attr=D.attr_list[a];
		if(value_domain[D.attr_list.size()][1].sum>value_domain[D.attr_list.size()][0].sum)
			parent->child[n]->result=1;
		else
			parent->child[n]->result=-1;
	}	
	else
	{
		parent->child=(node**)malloc(n*sizeof (node*));
		parent->child_num=n;
	}
			
	vector<subset> sub=divide_data(D,a,value_domain);
	
/**/for(int i=0;i<n;i++)
	{
	//	cout<<"第"<<i<<" 个:"<<endl; 
		parent->child[i]=new node;
		parent->child[i]->value=value_domain[a][i].val;
		//cout<<value_domain[a][i].val;
		parent->child[i]->attr=D.attr_list[a];
		vector<value> value_do[15];
		build_domain(sub[i],value_do);
		buildtree(sub[i],parent->child[i],value_do);
		//fout<<"kong"<<endl;
	}
}
node *root=new node;
void init()
{	
	root->attr=-1;
	root->value=0;
	subset D;
	for(int i=0;i<train_ent_num;i++)
	{
		D.ent_list.push_back(i);
		if(i<attr_num)
			D.attr_list.push_back(i);
	}
	
	vector<value> value_domain[15];
	build_domain(D,value_domain);
	buildtree(D,root,value_domain);
}
//ofstream fout("result_of_test.txt");
void judge(double set[max_ent_num][15],double tag[max_ent_num],int ent)
{
	int cnt=0;
	for(int k=0;k<ent;k++)
	{
		node **child=root->child;
		node *parent=root;
		int n=root->child_num;
		int result=0;
		while(child!=NULL)
		{
			int loop=0;
			for(int i=0;i<n;i++)
			{
				if(child[i]->value==set[k][child[i]->attr]||child[i]->value==-1)
				{
					if(child[i]->result==1||child[i]->result==-1)
						result=child[i]->result;
					n=child[i]->child_num;
					parent=child[i];
					child=child[i]->child;
					break;
				}
				loop++;
				if(loop==n)
				{
					if(parent->result>0)
						result=1;
					else
						result=-1;
					child=NULL;
					break;
					
				}
			}
		}
		//fout<<result<<endl;
		//<<" "<<set_tag[k]<<endl;
		if(result==tag[k])
			cnt++;
	}	
	cout<<"correct rate is "<<(double)cnt/(double)ent<<endl; 
		
}/**/
int main()
{
	getTrain();
	getValid();
	init();
	//judge(train_attr,train_tag,train_ent_num);
	judge(valid_attr,valid_tag,valid_ent_num); 
	//getTest();
	//judge(test_attr,test_tag,test_ent_num);
}
