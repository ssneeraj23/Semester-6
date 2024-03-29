#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#define bs 25

/* The client connect to the server a new connection is created and the same connection
   is used for all the expressions that is sent by the client
   and when the user enters -1 on client then client closes the connection and then server 
   waits for new accept.
*/
/*
  The server receives the expression in chunks of size "bs" as defined above and evaluates 
  the expression whenever a chunk is received. So, the expression can be of any size and we don
  't even need to store the entire expression to evaluate it.
*/
/*
  The fuctionality of server is written in such a way that as far as the teachings done are 
  considered, this code is as optimal as it can, to best of my knowledge
*/
int readFlt(char *buff, int true_size, float *ans)
{
    
    float value=0;
    float powerup=1;
    float powerdown=0.1;
    int dot=50;
    for(int i=0;i<true_size;++i)
    {
        if(buff[i]=='.')
        {
            dot=i;
            break;
        }
    }
    int point;
    if(dot==50)
    {
        point=true_size-1;
    }
    else point=dot-1;
    for(int i=point;i>=0;--i)
    {
         if(i==0)
         {
            if(buff[0]=='+'||buff[0]=='-')break;
            value=value+((int)(buff[i]-'0'))*powerup;
            break;
         }
         value=value+((int)(buff[i]-'0'))*powerup;
         powerup*=10;
    }
    point=dot+1;
    for(int i=point;i<=true_size-1;++i)
    {
        
        value=value+((float)(buff[i]-'0'))*powerdown;
        powerdown/=10;
    }
    if(buff[0]=='-')value*=-1;
    *ans=value;
    return value;
}


int main()
{

    char final_ans[20];
    char num[20];
    int newcon=1;
	int sockfd,newsockfd,clilen,rs;
    struct sockaddr_in	cli_addr, serv_addr;
    char exp[bs];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}
    serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(20000);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while(1)
    {
    for(int i=0;i<20;++i)final_ans[i]='\0';
    if(newcon==1)
    {
        newcon=0;
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen);
        if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}
    }    
    for(int i=0;i<bs;++i)exp[i]='\0';
    int j;
    int first_read=0,first_op=0,bfirst_op;
    char op;
    int num_read=0;
    int nums;
    float fval,curr_val;
    int bopen=0;
    int bf_read=0;
    char bf_op;
    float bfval,bcurr_val;
    while(1)
    {
        rs=recv(newsockfd,exp,bs,0);
        if(rs<=0){newcon=1;break;}
        for(int i=0;i<rs;++i)
        {
            if(exp[i]=='(')
                {
                    bopen=1;
                    bf_read=0;
                    bfirst_op=0;
                    continue;
                }
            if(bopen)
            {
                if(exp[i]==' ')continue;
                if(exp[i]==')')
            {
                bopen=0;
                 if(bfirst_op==0)
                {
                    curr_val=bcurr_val;
                    continue;
                }
                else
                {
                    if(bf_op=='+')bfval+=bcurr_val;
                    if(bf_op=='-')bfval-=bcurr_val;
                    if(bf_op=='*')bfval*=bcurr_val;
                    if(bf_op=='/')bfval/=bcurr_val;
                }
                curr_val=bfval;
                continue;
            }
                if((exp[i]=='+'||exp[i]=='-')||((exp[i]=='*'||exp[i]=='/')||exp[i]==')'))
                {
                    if(bfirst_op==0&&bf_read==0)
                {
                    bfirst_op=1;
                    bf_read=1;
                    bfval=0;
                }
                else
                {
                if(num_read==1)
                {
                    num_read=0;
                    readFlt(num,nums,&bcurr_val);
                        if(bf_read==0)
                        {
                            bf_read=1;
                            bfval=bcurr_val;
                        }
                }
                if(bfirst_op==0)
                {
                    bfirst_op=1;
                }
                else
                {
                    if(bf_op=='+')bfval+=bcurr_val;
                    if(bf_op=='-')bfval-=bcurr_val;
                    if(bf_op=='*')bfval*=bcurr_val;
                    if(bf_op=='/')bfval/=bcurr_val;
                }
                }
                bf_op=exp[i];
            }
            else
            {
                
                if(num_read==0)
                {
                    nums=0;
                    num_read=1;
                    for(j=i;j<rs;++j)
                    {
                        if((exp[j]>='0'&&exp[j]<='9')||exp[j]=='.')
                        {
                            num[nums]=exp[j];
                            ++nums;
                        }
                        else
                        {
                            num_read=0;
                            break;
                        }
                    }
                    i=j-1;
                    if(num_read==0)
                    {
                        readFlt(num,nums,&bcurr_val);
                        if(bf_read==0)
                        {
                            bf_read=1;
                            bfval=bcurr_val;
                        }
                    }
                }
                else{

                    for(j=i;j<rs;++j)
                    {
                        if((exp[j]>='0'&&exp[j]<='9')||exp[j]=='.')
                        {
                            num[nums]=exp[j];
                            ++nums;
                        }
                        else
                        {
                            num_read=0;
                            break;
                        }
                    }
                    i=j-1;
                    if(num_read==0)
                    {
                        readFlt(num,nums,&bcurr_val);
                        if(bf_read==0)
                        {
                            bf_read=1;
                            bfval=bcurr_val;
                        }
                    }
                }
            }
            continue;
            }
            if(exp[i]==' ')continue;
            if(exp[i]=='\0')
            {
                if(first_op==0)
                {
                    break;
                }
                else
                {
                    if(op=='+')fval+=curr_val;
                    if(op=='-')fval-=curr_val;
                    if(op=='*')fval*=curr_val;
                    if(op=='/')fval/=curr_val;
                }
                break;
            }
            if((exp[i]=='+'||exp[i]=='-')||(exp[i]=='*'||exp[i]=='/'))
            {
                if(first_op==0&&first_read==0)
                {
                    first_op=1;
                    first_read=1;
                    fval=0;
                }
                else
                {
                if(num_read==1)
                {
                    num_read=0;
                    readFlt(num,nums,&curr_val);
                        if(first_read==0)
                        {
                            first_read=1;
                            fval=curr_val;
                        }
                }
                if(first_op==0)
                {
                    first_op=1;
                }
                else
                {
                    if(op=='+')fval+=curr_val;
                    if(op=='-')fval-=curr_val;
                    if(op=='*')fval*=curr_val;
                    if(op=='/')fval/=curr_val;
                }
                }
                op=exp[i];
            }
            else
            {
                
                if(num_read==0)
                {
                    nums=0;
                    num_read=1;
                    for(j=i;j<rs;++j)
                    {
                        if((exp[j]>='0'&&exp[j]<='9')||exp[j]=='.')
                        {
                            num[nums]=exp[j];
                            ++nums;
                        }
                        else
                        {
                            num_read=0;
                            break;
                        }
                    }
                    i=j-1;
                    if(num_read==0)
                    {
                        readFlt(num,nums,&curr_val);
                        if(first_read==0)
                        {
                            first_read=1;
                            fval=curr_val;
                        }
                    }
                }
                else{

                    for(j=i;j<rs;++j)
                    {
                        if((exp[j]>='0'&&exp[j]<='9')||exp[j]=='.')
                        {
                            num[nums]=exp[j];
                            ++nums;
                        }
                        else
                        {
                            num_read=0;
                            break;
                        }
                    }
                    i=j-1;
                    if(num_read==0)
                    {
                        readFlt(num,nums,&curr_val);
                        if(first_read==0)
                        {
                            first_read=1;
                            fval=curr_val;
                        }
                    }
                }
            }
        }
        if(exp[rs-1]=='\0')
        {
            break;
        }
    }
    sprintf(final_ans,"%f",fval);
    send(newsockfd,final_ans,20,0);
    if(newcon)close(newsockfd);
    }
    close(sockfd);
	return 0;
}