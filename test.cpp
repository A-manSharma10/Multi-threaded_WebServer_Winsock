
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<pthread.h>
#include<malloc.h>
#include <fcntl.h>        
#include <sys/stat.h> 
#include<time.h>
#include<semaphore.h>
#include<limits.h>
#include<errno.h>
#include <dirent.h> 
#include <pwd.h>
#define BUF_SIZE 2048

#define SUCCESS_IMAGE    "HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define SUCCESS_TEXT     "HTTP/1.0 200 OK\nContent-Type:text/html\n\n"
#define NOT_SUCCESS_404   "HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404    "<html><body><h1>FILE NOT FOUND</h1>"
#define DIR_404    "<html><body><h1> Directory not FOUND </h1>"
#define USER_404    "\nUser not found\n"

sem_t sem;
int free_thread;
int sched_flag=0,debug_flag=0,log_flag=0;
char * file=NULL;
char *dir=NULL;
int isUser=0;
pthread_t t_serve;
pthread_mutex_t qmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sthread_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;


struct request
{
	int acceptfd;
	int size;
	char incoming_req[1024];
	char req_type[1024];
	char file_name[1024];
	unsigned int cli_ipaddr;
	char time_arrival[1024];
	char in_buf[2048];
	
}r2; 

// queue function declarations;
void insertion(int,char*, int, unsigned int,char*,char*,char*,char*);	
//void insertion(int,string, int);
struct request extract_element();
struct request removesjf();
void display();
void print_help_options();
int exits(char *f);
void sort();
void listDir();
char tem[1024];



//queue structre
struct node
{
	struct request r;	
	struct node *link;
}*new,*temp,*p,*front=NULL,*rear=NULL,*temp1;
typedef struct node N;

struct node* swap(struct node* l1,struct node* l2);

//Display the contents of the queue
void display()
{
	if(front==NULL)
		printf("\nempty queue");
	else
	{
		int a;
		
		temp=front;
		while(temp!=NULL)
		{
			a=(temp->r.acceptfd);			
			printf("\n acceptfd is %d, file name is %s, file size is %d , ip addr is %u, request is %s,time is %s type %s incoming %s",a,temp->r.file_name,temp->r.size,temp->r.cli_ipaddr,temp->r.in_buf,temp->r.time_arrival,temp->r.req_type,temp->r.incoming_req);
			temp=temp->link;
		}
	}
} 


void display1()
{
	if(front == NULL)
		printf("\n empty..");
	
	temp = front;

	while(temp != NULL)
	{
		printf("%d[%d]->",temp->r.acceptfd,temp->r.size);
		temp = temp->link;
	}
}
//end


// queue functions
void insertion(int afd,char *f,int size,unsigned int ip, char * time_arrival,char * in_buf,char *incoming,char *type)
{

	
	new=(N*)malloc(sizeof(N));
	int n;
	char a[1024];
	char b[1024];
	char c[1024];
	char d[1024];
	char e[1024];
	strcpy(a,f);
	strcpy(b,time_arrival);
	strcpy(c,in_buf);
	strcpy(d,incoming);
	strcpy(e,type);
	new->r.acceptfd=afd;
	strcpy(new->r.file_name,a);
	new->r.cli_ipaddr=ip;
	strcpy(new->r.time_arrival,b);
	strcpy(new->r.in_buf,c);
	strcpy(new->r.incoming_req,d);
	strcpy(new->r.req_type,e);
	new->r.size=size;
	new->link=NULL;
	printf("\n Inside insertion inc %s type %s\n",incoming,type);
	if(front==NULL)
		front=new;
	else
		rear->link=new;
		rear=new;
	
	display();		
}
//end




//Removing the element for SJF
struct request removesjf(int num)
{ 
	
	if(front==NULL)
	{
		 printf("\n\nempty list");
	}
	else
	{
		struct node *old,*temp;
		temp=front;
		while(temp!=NULL)
		{ 
			if(temp->r.acceptfd==num)
			{ 
				if(temp==front) 
				front=temp->link; 
				else 
				old->link=temp->link;
				return(temp->r);
				free(temp);
				
			}
			else
			{ 
				old=temp;
				temp=temp->link;
			}
		}
	}
}

void sort()
{
	
	struct node *p, *q, *r, *s, *temp ;
    s = NULL ;

    /* r precedes p and s points to the node up to which comparisons are to be made */
while ( s != front -> link )
    {
        r = p = front ;
        q = p -> link ;

        while ( p != s )
        {
            if ( p -> r.size >= q -> r.size )
            {
                if ( p == front )
                {
                    temp = q -> link ;
                    q -> link = p ;
                    p -> link = temp ;

                    front = q ;
                    r = q ;
                }
                else
                {
                    temp = q -> link ;
                    q -> link = p ;
                    p -> link = temp ;

                    r -> link = q ;
                    r = q ;
                }
            }
            else
            {
                r = p ;
                p = p -> link ;
            }
            q = p -> link ;
            if ( q == s )
                s = p ;
        }
    }

			
}


struct node* swap(struct node *l1,struct node *l2)
{
	l1->link = l2->link;
	l2->link = l1;
	return l2;
}
//end

//count the no of slashes
int countSlash(char *str)
{
	int count = 0;
	while(*str != '\0')
	{
		
		if(*str == '\/')
		{
			count++;
		}
		str++;
	}	
	return count;
}
//end


//check for a dot
int isThere(char *str)
{
	int c=0;
	while(*str != '\0')
	{
		
		if(*str == '.')
		{
			c++;
			break;
		}    
		str++;
	}
	
	return c;
}
//end 

//changing the directory
char* changeDir(char *str)
{
   
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
      			perror("getcwd() error");
    
    char *file1;
    file1=(char *)malloc(1024*sizeof(char));
    strcpy(file1,cwd);
    strcat(file1,str);
   
 
    return file1;
}
//end


//changing the file name
char* changeFN(char *str1)
{
	
   int a=0;
   char *token;
   char result[1024];
   
   
   token = strtok(str1, "/");
   strcpy(result,"/");
  
   while( token != NULL ) 
   {
    
      a = isThere(token);
      
      if(a==0)
	{
		strcat(result,token);
		strcat(result,"/");
	}
	if(a==1)
	{
		break;
	}
      token = strtok(NULL, "/");
      //printf("token %s\n",token);
   }
	//printf("change name %s\n",result);
	return result;
}
//end


//Listing the files of the directory
void listDir1(char *str1)
{
	DIR           *d;
	struct dirent *dir;
	char *str2 = changeFN(str1);
	
	d = opendir(str2);
  char str[100][100];
  char temp[100];
  int i=0,j=0,k=0;
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
     
      strcpy(str[i],dir->d_name);
      
	i++;
    }
  
   
    closedir(d);
  }

	for(k=0;k<i;k++)
		for(j=k+1;j<i;j++)
		{
			if(strcmp(str[k],str[j])>0)
			{
				
				strcpy(temp,str[j]);
				strcpy(str[j],str[k]);
				strcpy(str[k],temp);
				
			}
		}
	for(j=0;j<i;j++)
	{
		if((strcmp(str[j],".") == 0) || (strcmp(str[j],"..") == 0))
			continue;
		strcat(tem,str[j]);
		strcat(tem,"\n");
	}
}
//end

//Listing the files of the directory
void listDir()
{
	DIR           *d;
	struct dirent *dir;
	d = opendir(".");
  char str[100][100];
  char temp[100];
  int i=0,j=0,k=0;
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
     
      strcpy(str[i],dir->d_name);
      
	i++;
    }
  
  
    closedir(d);
  }

	for(k=0;k<i;k++)
		for(j=k+1;j<i;j++)
		{
			if(strcmp(str[k],str[j])>0)
			{
				
				strcpy(temp,str[j]);
				strcpy(str[j],str[k]);
				strcpy(str[k],temp);
				
			}
		}
	for(j=0;j<i;j++)
	{
		if((strcmp(str[j],".") == 0) || (strcmp(str[j],"..") == 0))
			continue;
		strcat(tem,str[j]);
		strcat(tem,"\n");
	}
}
//end

//Listing the files of the directory
void listDirTilde(char* str1)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir(str1);
  char str[100][100];
  char temp[100];
  int i=0,j=0,k=0;
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
     
      strcpy(str[i],dir->d_name);
      
	i++;
    }
  
  
    closedir(d);
  }

	for(k=0;k<i;k++)
		for(j=k+1;j<i;j++)
		{
			if(strcmp(str[k],str[j])>0)
			{
				
				strcpy(temp,str[j]);
				strcpy(str[j],str[k]);
				strcpy(str[k],temp);
				
			}
		}
	for(j=0;j<i;j++)
	{
		if((strcmp(str[j],".") == 0) || (strcmp(str[j],"..") == 0))
			continue;
		strcat(tem,str[j]);
		strcat(tem,"\n");
	}
}
//end

//checking for a tilde
int isTilde(char *str)
{
	char * pch;
 	int tilde=0;
        //printf("Inside isTilde %s\n",str);
 	pch=strchr(str,'~');
  	while (pch!=NULL)
  	{
	
	  tilde =1;
	  break;
  	 
  	}
	
	return tilde;
}
//end


char* tilde(char *str)
{

	
	char *token;
	int tilde=0;
        char c[1024];
	char *buf;
	buf=(char *)malloc(10*sizeof(char));
	buf=getlogin();
	
	strcpy(c,"/home/");
	strcat(c,buf);
	strcat(c,"/myhttpd/");
	
	DIR* dir = opendir(c);
	if (dir)
	{
 	   printf("directory exits\n");
 	   closedir(dir);
	}
	else
	{
	   printf("No directory\n");
	   return NULL;
	}
	
	
  	 token = strtok(str, "/");
	
        int a=0;
  	
 	  while( token != NULL ) 
  	 {
		 
		if(strcmp(token,"~") == 0)
		{	
			token = strtok(NULL, "/");
			continue;
		}

		a = isThere(token);
		if(a==1)
		{
			strcat(c,token);
			break;
		}		

		strcat(c,token);
		strcat(c,"/");
		token = strtok(NULL, "/");
	 }
	
        return c;
}


//checking for the second slash
int slash(char *str)
{
	char * pch;
 	int a=0,c=0;

 	pch=strchr(str,'/');
  while (pch!=NULL)
  {
    a++;
    c = pch-str;
   
    if(a==2)
	return c;
    pch=strchr(pch+1,'/');
  }
	return c;
}
//end


//find the occurence of tilde
int findTilde(char* str)
{
	int c=0;
	char * pch;
	pch=strchr(str,'~');
	c = pch-str+1;
	return c;
}
//end

//getting the user using getpwnam
char* getUser(char *user)
{
	struct passwd *p;
	char* te[1024];
  	

  	if ((p = getpwnam(user)) == NULL)
	{
    		perror("getpwnam() error");
		strcpy(te,"user");
		printf("te %s\n",te);
		isUser=1;
		
	}
 	 else 
	{
   		 
    		printf("  pw_dir   : %s\n",       p->pw_dir);
    		return p->pw_dir;

	}
return te;
}
//end


//changing the file name for tilde
char* changeTilde(char *str)
{
	int tildes=0;
  	int sl=0;
        int i=0,j=0;
 	char te[1024];
	char te1[1024];
	char tem[1024];
	char c[1024];
	DIR* dir = opendir(c);
	memset(c,0,sizeof(c));
        printf("\nC = %s\n",c);
	
  
//pch=strchr(str,'~');
  	tildes = findTilde(str);;
  	sl = slash(str);
  	
  	for(i=tildes;i<sl;i++)
  	{
		
		te[j++]=str[i];
 	}
		te[j]='\0';
		strcpy(te1,getUser(te));
		
		if(strcmp(te1,"user") != 0)
 	 		strcpy(te,getUser(te));
		printf("%s\n",te1);
 		printf("Tilde str file %s\n",te);
		j=0;
 	 for(i=sl+1;i<strlen(str);i++)
  	{
	
		tem[j++]=str[i];
  	}
		tem[j]='\0';
		strcat(c,te);
		strcat(c,"/myhttpd/");
		
		if (c)
		{
 		   printf("directory exits\n");
 		   closedir(dir);
		}
		else
		{
		   printf("No directory\n");
		   return NULL;
		}
		strcat(c,tem);
		te[0]='\0';
		te1[0]='\0';
		tem[0]='\0';
		printf("Tilde str file 2 %s\n",c);
	return c;
}
//end


//adding index.html for tilde
char* changeTildeIndex(char *str)
{
	
	int tildes=0;
  	int sl=0;
        int i=0,j=0;
 	char te[1024];
	char te1[1024];
	char tem[1024];
	char c[1024];
	DIR* dir = opendir(c);
	memset(c,0,sizeof(c));
        //strcpy(c,"/home/");
	
  

  	tildes = findTilde(str);;
  	sl = slash(str);
  	
  	for(i=tildes;i<sl;i++)
  	{
		
		te[j++]=str[i];
 	}
		te[j]='\0';
		strcpy(te1,getUser(te));
		
		if(strcmp(te1,"user") != 0)
 	 		strcpy(te,getUser(te));
		printf("Tilde %s\n",te1);
		j=0;
 	 for(i=sl+1;i<=strlen(str);i++)
  	{
	
		tem[j++]=str[i];
  	}
		tem[j]='\0';
	//printf("Tilde str 1 %s\n",tem);
		strcat(c,te);
		strcat(c,"/myhttpd/");
		strcat(c,tem);
		strcat(c,"index.html");

		if (c)
		{
 		   printf("directory exits\n");
 		   closedir(dir);
		}
		else
		{
		   printf("No directory\n");
		   return NULL;
		}
		te[0]='\0';
		te1[0]='\0';
		tem[0]='\0';
	//printf("Tilde str 2 %s\n",c);
		
	return c;
}
//end

//change to index.html for '/'
char* changeIndex(char *str)
{
   
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
      			perror("getcwd() error");
    
    char *file1;
    file1=(char *)malloc(1024*sizeof(char));
    strcpy(file1,cwd);
    strcat(file1,str);
    strcat(file1,"index.html");
   
 
    return file1;
}
//end
//change to index.html for '/'
char* changeIndex1(char *str)
{
   
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
      			perror("getcwd() error");
    
    char *file1;
    file1=(char *)malloc(1024*sizeof(char));
    strcpy(file1,cwd);
    strcat(file1,str);
   
   
 
    return file1;
}
//end
//Transform
char* trans(char *str)
{
	int istilde1=0;
	int dot=0,slash=0;
	dot = isThere(str);
		slash = countSlash(str);
		istilde1=isTilde(str);
		
		
		
		if((dot ==0) && (slash > 0 ) && istilde1 ==0)
		{
			
			str = changeIndex(str);
				
		}
		else if((dot ==0) && (slash > 0 ) && istilde1 ==1)
		{
			
			str = changeTildeIndex(str);
		}
		else if(istilde1 == 1)
		{
			str = changeTilde(str);	
		}
		else if(istilde1==0 && dot == 1 && slash>0)
		{
			str = changeIndex1(str);	
		}
return str;
}
//end
 struct request extract_element()
{
	char b[1024];
	char c[1024];
	char d[1024];
	char e[1024];

	if(front==NULL)
		printf("\nempty queue");
	else
	{	
		struct request r1;
		p=front;
		
		front=front->link;
		r1.acceptfd=p->r.acceptfd;
		strcpy(r1.file_name,p->r.file_name);		
		r1.size=p->r.size;
		strcpy(b,p->r.time_arrival);
		strcpy(c,p->r.in_buf);
		strcpy(d,p->r.req_type);
		strcpy(e,p->r.incoming_req);
		strcpy(r1.time_arrival,b);
		strcpy(r1.in_buf,c);
		strcpy(r1.req_type,d);
		strcpy(r1.incoming_req,e);

		free(p);
		
		return(r1);
	}
}
 
// thread pool function

void *thread_serve()
{
	
	while(1)
	{		
		pthread_mutex_lock(&sthread_mutex);		
		pthread_cond_wait(&cond_var,&sthread_mutex);
		struct request r=r2;
		pthread_mutex_unlock(&sthread_mutex);
		time_t now;					// getting the time the job has been assigned to the serving thread
                time(&now);
       	        struct tm * ct=localtime(&now); //getting localtime
       	        int ch[128], time_serve[128];
       	        struct timeval tv;
       	        strftime(ch, sizeof ch, "[%d/%b/%Y : %H:%M:%S %z]", ct); //format of the timestamp string we need
       	        snprintf(time_serve, sizeof time_serve, ch, tv.tv_usec); //printing the needed timestamp string
		unsigned int ip=r.cli_ipaddr;
  
    /* code adapted from stackoverflow.com */
    
		unsigned char bytes[4];
		bytes[0] = ip & 0xFF;
		bytes[1] = (ip >> 8) & 0xFF;
		bytes[2] = (ip >> 16) & 0xFF;
		bytes[3] = (ip >> 24) & 0xFF;
 /* end of code adapted from stackoverflow.com */

			/*if(debug_flag==0&& log_flag==1)
			{
				FILE * file_des=fopen(file,"a"); 
				fprintf(file_des,"%d.%d.%d.%d\t-\t ", bytes[0], bytes[1], bytes[2], bytes[3]);
				fprintf(file_des,"%s\t %s\t %s \t 200 \t %d\n\n",r.time_arrival,time_serve,r.incoming_req,r.size);
				fclose(file_des);
			}
              
			else if(debug_flag==1)
			{  
				printf("\n%d.%d.%d.%d\t-\t %s\t %s\t %s \t 200 \t %d\n", bytes[0], bytes[1], bytes[2], bytes[3],r.time_arrival,time_serve,r.in_buf,r.size);
			}*/
		
		char           in_buf[BUF_SIZE];      
		char           out_buf[BUF_SIZE];
		char           out_buf1[BUF_SIZE];
		char typ[1024];
		char           *file_name;                 
		file_name=malloc(sizeof(char *));
		char           *file_name1;                 
		file_name1=malloc(sizeof(char *));
		char           *file_name2;                 
		file_name2=malloc(sizeof(char *));
		int acceptfd;
		unsigned int   fd1;                         
		unsigned int   buffer_length;                    
		unsigned int   retcode;  
		int m; 
		int isD=0;
		int cslash=0;
                int istilde = 0;
		int istilde1 = 0;
		int dot=0;
		int slash =0;
		int onlyS=0;
		int onlyT=0;
		char getTime[1024];
		char size1[1024];
		struct stat attr;
		struct stat st; 
		int siz=0;

		acceptfd=r.acceptfd;
		file_name=r.file_name;
		strcpy(typ,r.req_type);
		//new
		dot = isThere(file_name);
		slash = countSlash(file_name);
		istilde1=isTilde(file_name);
		
		
		if((dot ==0) && (slash > 0 ) && istilde1 ==1)
		{
			printf("1\n");
			file_name = changeTildeIndex(file_name);
			
				fd1 = open(file_name, O_RDONLY, S_IREAD | S_IWRITE);
			
			onlyT=1;
			
		}
		
		if((dot ==0) && (slash > 0) && (onlyT!=1))
		{
			printf("2\n");
			strcpy(file_name,changeIndex(file_name));
                        printf("Modified file name %s\n",file_name);
			
				fd1 = open(file_name, O_RDONLY, S_IREAD | S_IWRITE);
			
			onlyS=1;
		}
		//
		{	
				istilde = isTilde(file_name);
			
			if(istilde == 1 && onlyT!=1)
			{
				printf("3\n");
				file_name1 = changeTilde(file_name);
				
				if(file_name1 != NULL)
				{
					printf("F1 =%s\n",file_name1);
					fd1 = open(file_name1, O_RDONLY, S_IREAD | S_IWRITE);
				}
				else
				{	
					printf("NULL DIR\n");
					isD=1;
					fd1=-1;
				}
			
			}
		
			cslash = countSlash(file_name);
		if(cslash>1 && istilde!=1 && onlyS!=1 && onlyT!=1)
		{
			printf("4\n");
                        file_name1 = changeDir(file_name);
			fd1 = open(file_name1, O_RDONLY, S_IREAD | S_IWRITE); 
		}
		else	
		{
			printf("5\n");
			if(istilde!=1 && onlyS!=1 && onlyT!=1)
			{
				printf("5a\n");
				fd1 = open(&file_name[1], O_RDONLY, S_IREAD | S_IWRITE);
			}
			
		}
 		memset(out_buf, 0, sizeof(out_buf));
        	if (fd1 == -1)
        	{
			char result[1024];
			printf("File %s not found - sending an HTTP 404 \n", &file_name[1]);
        		strcpy(out_buf, NOT_SUCCESS_404);
			
        		send(acceptfd, out_buf, strlen(out_buf), 0);
			
			if((cslash>1 || istilde==1))
			{
					printf("6\n");
					printf("%d ony T",onlyT);
					if(onlyT == 1)
						listDir1(file_name);
					else
						listDir1(file_name1);
					
					
				
			}
			
			else
			{
				printf("8\n");
				listDir();
			}
			if(isD==1 || isUser==1)
			{
				if(isD==1)
					strcpy(out_buf, DIR_404);
				else
					strcpy(out_buf, USER_404);
			}
			else
			{
				strcpy(result,"FILE NOT FOUND");
				strcat(result,"\n The files in the directory are\n");
				strcat(result,tem);
		      		strcpy(out_buf, result);
			}
				send(acceptfd, out_buf, strlen(out_buf), 0);
//checking

			if(debug_flag==0&& log_flag==1)
			{
				FILE * file_des=fopen(file,"a"); 
				fprintf(file_des,"%d.%d.%d.%d\t- ", bytes[0], bytes[1], bytes[2], bytes[3]);
				fprintf(file_des,"%s %s %s  404 0\n\n",r.time_arrival,time_serve,r.incoming_req,r.size);
				fclose(file_des);
			}
              
			else if(debug_flag==1)
			{  
				printf("\n%d.%d.%d.%d\t- %s %s %s 404 0 \n", bytes[0], bytes[1], bytes[2], bytes[3],r.time_arrival,time_serve,r.in_buf,r.size);
			}

//end	
				tem[0]='\0';
				isD=0;
				cslash=0;
                		istilde = 0;
				istilde1 = 0;
				dot=0;
				slash =0;
				onlyS=0;
				onlyT=0;
				isUser=0;
			/*if(debug_flag==0&& log_flag==1)
			{
			
				FILE * file_des=fopen(file,"a"); 
				fprintf(file_des,"%d.%d.%d.%d\t- ", bytes[0], bytes[1], bytes[2], bytes[3]);
				fprintf(file_des,"%s %s %s  404  %d\n\n",r.time_arrival,time_serve,r.in_buf,r.size);
				fclose(file_des);
			}
              
			else if(debug_flag==1)
			{  
				printf("\n%d.%d.%d.%d\t-\t %s\t %s\t %s \t 404 \t %d\n", bytes[0], bytes[1], bytes[2], bytes[3],r.time_arrival,time_serve,r.in_buf,r.size);
			}*/

		}
	        else
	        {
			//printf("\n\nTYPE =%s\n\n",typ);
				printf("FFF= %s\n",file_name);
				stat(file_name,&attr);
				strftime(getTime,sizeof getTime,"[%e/%b/%G : %H:%M:%S %z]",gmtime(&attr.st_mtime));
				printf("MF %s\n",getTime);
				strcpy(out_buf1,"Date: ");
				
				strcat(out_buf1,time_serve);
				strcat(out_buf1,"\n");
				strcat(out_buf1,"Server : Myhttpd 1.0\n");
				strcat(out_buf1,"Modified Time : ");
				if(strstr(getTime,"1970") != NULL)
				{
					stat(file_name1,&attr);		
					printf("FFF12= %s\n",file_name1);	
					strftime(getTime,sizeof getTime,"[%e/%b/%G : %H:%M:%S %z]",gmtime(&attr.st_mtime));
					strcat(out_buf1,getTime);

					
				}
				else
				{
					strcat(out_buf1,getTime);
					
					sprintf(size1,"%d",siz);
				}
				strcat(out_buf1,"\n");
				printf("%s\n",out_buf1);
				
				if(r.size > 0)
				{
					siz = r.size;

				}
				else
				{
					if(file_name1 != NULL)
						if (stat(file_name1, &st) == 0)
			    				siz=st.st_size;
					else
					{
						if (stat(file_name, &st) == 0)
			    				siz=st.st_size;
					}
				}
				printf("\n s %d \n",siz);
				if((strcmp(typ,"GET"))==0)
				{
					
					sprintf(size1,"%d",siz);
					printf("File %s is being sent \n", &file_name[1]);
					if ((strstr(file_name, ".jpg") != NULL)||(strstr(file_name, ".gif") != NULL)) 
					 { 
						
						strcpy(out_buf, SUCCESS_IMAGE); 
					 }
					else
					  { 
						printf("sasa\n");
						
						strcat(out_buf1,"ContentLength: ");
						strcat(out_buf1,size1);	
						strcat(out_buf1, "\n");
						strcpy(out_buf, out_buf1); 
						strcat(out_buf, SUCCESS_TEXT);  		
						printf("%s\n",out_buf);
					  }
					  send(acceptfd, out_buf, strlen(out_buf), 0);
		 			  //memset(out_buf,0,sizeof(out_buf));
					  out_buf[0]='\0';
					  buffer_length = 1;  
					  while (buffer_length > 0)  
					  {
						    buffer_length = read(fd1, out_buf, BUF_SIZE);
						    if (buffer_length > 0)   
				   		    { 
			     				 send(acceptfd, out_buf, buffer_length, 0);     
			   			    }
			 		  }
				}
				else
				{
					if ((strstr(file_name, ".jpg") != NULL)||(strstr(file_name, ".gif") != NULL)) 
					 { 
						strcpy(out_buf, SUCCESS_IMAGE); 
					 }
					else
					  { 
						strcpy(out_buf, SUCCESS_TEXT); 
					  }
					  send(acceptfd, out_buf, strlen(out_buf), 0);
				}
//checking

			if(debug_flag==0&& log_flag==1)
			{
				FILE * file_des=fopen(file,"a"); 
				fprintf(file_des,"%d.%d.%d.%d\t- ", bytes[0], bytes[1], bytes[2], bytes[3]);
				fprintf(file_des,"%s %s %s 200 %s\n\n",r.time_arrival,time_serve,r.incoming_req,size1);
				fclose(file_des);
			}
              
			else if(debug_flag==1)
			{  
				printf("\n%d.%d.%d.%d\t- %s %s %s 200 %s\n", bytes[0], bytes[1], bytes[2], bytes[3],r.time_arrival,time_serve,r.in_buf,size1);
			}

//end		


			
		}
		printf("var isd %d cslash %d istilde %d istilde1 %d dot %d slash %d onlyS%d  onlyT%d user %d",isD,cslash,istilde,istilde1,dot,slash,onlyS,onlyT,isUser); 

		
		close(fd1);
		close(acceptfd);	
	}


}
printf("out of server");
}


//scheduler thread
void *thread_scheduler(void *arg)
{
	printf("\n In schedule \n");
	unsigned int schedalg=*((unsigned int*)arg);
	int acceptfd,n;
	
	if(schedalg==0)
	{	
		while(1)
		{	
			if(front!=NULL)
			{	
				
				sem_wait(&sem);				
				pthread_mutex_lock(&sthread_mutex);
				pthread_mutex_lock(&qmutex);
				r2=extract_element();
				pthread_mutex_unlock(&qmutex);
				pthread_cond_signal(&cond_var);
				free_thread--;
				pthread_mutex_unlock(&sthread_mutex);	
				sem_post(&sem);			
			}
			else 
			{		
				continue;
			}			
		}
	}
	if(schedalg==1)
	{
	//code for SJF scheduling algorithm
		
		printf("\n entered SJF scheduling algorithm");
		int shortestjob_fd=0;
		int min = INT_MAX;
		int a,b;
		while(1)			
		{	
			if(front!=NULL)
			{	
				
				
			sem_wait(&sem);	
			pthread_mutex_lock(&sthread_mutex);
			pthread_mutex_lock(&qmutex);
			sort();
			display1();

			r2=extract_element();
			
			pthread_mutex_unlock(&qmutex);
			
			
			pthread_cond_signal(&cond_var);
			free_thread--;
			pthread_mutex_unlock(&sthread_mutex);
			sem_post(&sem);
			}
		else
		{
			continue;
		}			
			
			
		}
	
	}
	
}


int exits(char *file)
{	
	
	
	
	struct stat buf;
	if(stat(file,&buf)==0)
	{
		
		return 1;
	}
	return 0;
}


// Listening and queueing thread

void *thread_listen(void *arg)
{
	unsigned int sockfd=*((unsigned int*)arg);
	int i,size;	
	unsigned int acceptfd,ids2;
	socklen_t clilen;
	int newsockfd[10],c;
	int n;
	char buffer[256];
	pthread_t t_serve[10];
	struct sockaddr_in cli_addr;	
	clilen = sizeof(cli_addr);
	unsigned int retval;
	char request_buffer[1024];
	int retcode;
	off_t file_size;
	off_t file_size1;
	char in_buf[BUF_SIZE];
        char reqLine[1024];
        char incoming[1024];
	char type[1024];
	struct stat st; 

	int k,j;
	int l;
	
	listen(sockfd,10);								// listens
	while(1)     
	
	{	
		
		printf("\nin listening thread before accept\n");
		acceptfd= accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
		if (acceptfd < 0) 
          	perror("error in accepting");
    		unsigned int ip=cli_addr.sin_addr.s_addr;
		time_t now;
                time(&now);
                struct tm * ct=localtime(&now); //getting localtime
                int ch[128], time_arrival[128];
                struct timeval tv;
                strftime(ch, sizeof ch, "[%d/%b/%Y : %H:%M:%S %z]", ct); //format of the timestamp string we need
                snprintf(time_arrival, sizeof time_arrival, ch, tv.tv_usec); //printing the needed timestamp string
               	char *file_name=malloc(sizeof(char *));
		char *file_name1=malloc(sizeof(char *));

		 memset(in_buf, 0, sizeof(in_buf));
		retcode = recv(acceptfd, in_buf, BUF_SIZE, 0);	
		strcpy(incoming,strtok(strdup(in_buf),"\r\n"));
		//strcpy(incoming,in_buf);		
		if (retcode < 0)
		{ 
			printf("recv error detected ...\n"); 
		}
     		else
		{    							
			strtok(in_buf, " ");
		        file_name = strtok(NULL, " ");	
			strcpy(type,in_buf);		
		}
		if(file_name!=NULL)
		{
			file_name1 = trans(file_name);
			printf("\n FIlename = %s\n",file_name1);
			if (stat(file_name1, &st) == 0)
			{
			    file_size1=st.st_size;
			    printf("SSS = %d\n",file_size);
				st.st_size = 0;
			}
			else
			{
				file_size1=0;
				printf("SSS = %d\n",file_size);
				
			}
			char *fname=malloc(sizeof(char *));
			k=1,j=0; 
			while(k<strlen(file_name))
			{
				fname[j]=file_name[k];
				
				k++;
				j++;			
			}
					
			    	
			
			if(exits(&file_name[1]))
				printf("\n there \n");
			
			
			    if (stat(&file_name[1], &st) == 0)
			{
			    file_size=st.st_size;
			    
				st.st_size = 0;
			}
			else
			{
				file_size=0;
				
			}
			insertion(acceptfd,file_name,file_size1,ip,time_arrival,in_buf,incoming,type);
			display1();
		}
		else
		{
			continue;
		}		
	}				 
}

int convertNumber(char *st)
{
	int base;
    char *endptr, *str;
    long val;

   

   str = st;
   
   errno = 0;    /* To distinguish success/failure after call */
    val = strtol(str, &endptr, 10);

   /* Check for various possible errors */

   if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            || (errno != 0 && val == 0)) {
        perror("strtol");
      
    }

   if (endptr == str) {
        fprintf(stderr, "No digits were found.\n");
	val =0;
      
    }

   /* If we got here, strtol() successfully parsed a number */

   

   if (*endptr != '\0')        /* Not necessarily an error... */
        printf("Further characters after number: %s\n", endptr);

	return val;
   
}

int main(int argc, char *args[])
{
	int thread_status[10];     	
	pthread_t t_listener,t_scheduler,t_serve[10];
	int sockfd,ids;
	char *dir;
	file=malloc(sizeof(char *));
	dir=malloc(sizeof(char *));
	int option =1;
	

	int portnum=8080,threadnum=4,sleep_time=60;
	int i;
	int help_flag=0,dir_flag=0,time_flag,threadnum_flag=0;
	
	while ((option = getopt(argc, args,"dhl:p:r:t:n:s:")) != -1) {
        switch (option) {
		
		case 'h' :
				help_flag=1;
				break;
		case 'n' :
				threadnum=convertNumber(optarg);
				if(threadnum == 0)
					printf("Please enter the threadnum in digits\n");
				break;
		case 'd' :
				debug_flag=1;
				threadnum=1;	
				break;
		case 'l' :
				log_flag=1;
				file=optarg;
				break;
		case 'p' :
				portnum=convertNumber(optarg);
				if(portnum == 0)
					printf("Enter the port number Correctly\n");
				break;
		case 'r' :
				dir_flag=1;		
				dir=optarg;
				break;
		case 't' :
				time_flag=1;		
		 		sleep_time=convertNumber(optarg);
				if(sleep_time == 0)
					printf("Enter the time properly in seconds\n");
				break;
		case 's' :	
				if (strcmp(optarg,"FCFS")==0)		
					sched_flag=0;
				else if(optarg,"SJF")
					sched_flag=1;
				else
					printf("Please enter a proper scheduling algorithm");
				break;
		default: 	
				print_help_options(); 
                		exit(EXIT_FAILURE);
	
	}
	}
	
	sem_init(&sem,0,threadnum);



	if(help_flag==1)			// printing help options and exit if -h option is specified
	{
		print_help_options();
		exit(1);
	}
	else if(dir_flag==1)			//changing directory if -d option is specified
	{
		if(chdir(dir)<0)
			{
				perror("\ndirectory doesnt exist");
				exit(1);
			}
	}
	
	if(debug_flag != 1)
	{
		daemon(0,0);
	}
	


	
	struct sockaddr_in serv_addr;
	
	sockfd = socket(AF_INET, SOCK_STREAM,0);			//creation of socket	
	
	if (sockfd < 0) 
	{	
        	perror("error creating socket");
		exit(1);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port =htons(portnum);

	if(setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR,(char*)&option,sizeof(option)) < 0)
	{
		printf("setsockopt failed\n");
		close(sockfd);
		exit(2);
	}

	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)		//binding socket
	{	
		perror("binding error");
		exit(1);
	}
	int w;
	ids=sockfd;
	pthread_create(&t_listener,NULL,&thread_listen,&ids);			//creating listener thread
	for(w=0;w<threadnum;w++)
	{
		pthread_create(&t_serve[w],NULL,&thread_serve,NULL); 
     	} 
	sleep(sleep_time);							// putting scheduler to sleep
	pthread_create(&t_scheduler,NULL,&thread_scheduler,&sched_flag);	//creating scheduler thread
	int a = pthread_kill(t_scheduler,0);
	if( a == ESRCH)
	printf("\n err = %d",a);       
	pthread_join(t_listener,NULL);
	pthread_join(t_scheduler,NULL);
	display();
	close(sockfd);
	return 0;
}


void print_help_options()
{
printf("\nâˆ’d : Enter debugging mode. That is, do not daemonize, only accept one connection at a \ntime and enable logging to stdout. Without this option, the web server should run as a daemon process in the background. \nâˆ’h : Print a usage summary with all options and exit. \nâˆ’l file : Log all requests to the given file. See LOGGING for details.\nâˆ’p port : Listen on the given port. If not provided, myhttpd will listen on port 8080. \nâˆ’r dir : Set the root directory for the http server to dir. \nâˆ’t time : Set the queuing time to time seconds. The default should be 60 seconds. \nâˆ’n threadnum: Set number of threads waiting ready in the execution thread pool to threadnum. \nThe default should be 4 execution threads. \nâˆ’s sched: Set the scheduling policy. It can be either FCFS or SJF. The default will be FCFS.");

}














	