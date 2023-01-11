#include <stdio.h>

#define MYL H
#define ERR 0
#define OK 1
int readInt(int *n)
{
    int max_size=15;
    int true_size=0;
    char buff[15];
    for(int i=0;i<15;++i)buff[i]='\0';
    // printf("hello \n");
    scanf("%s",buff);
    // printf("buff is %s \n",buff);
    for(int i=0;i<15;++i)
    {
        // printf("true size is %d\n",true_size);
        if(buff[i]!='\0')true_size=i+1;
        else break;
    }
    // printf("true size is %d\n",true_size);
    ++true_size;
    if(true_size<=1)return ERR;        
    if(true_size>11)return ERR;       //if read string has more size than int can accept then return error
    int btest;
    for(btest=0;btest<true_size-1;++btest)
    {
        if(buff[btest]>='0'&&buff[btest]<='9')break;
    }
    if(btest==true_size-1)return ERR;   //if there is no number in string then return error
    if((buff[0]!='+'&&buff[0]!='-')&&(buff[0]<'0'||buff[0]>'9'))return ERR; //if the first char is other than +,- or a number then return error
    for(btest=1;btest<true_size-1;++btest)  //if after first char there is a non numeric char then return error
    {
        if(buff[btest]<'0'||buff[btest]>'9')break;  
    }   
    if(btest!=true_size-1)return ERR;
    int is_positive=1;
    if(buff[0]=='-')is_positive=0;
    int value=0;
    int power=1;
    for(int i=true_size-2;i>0;--i)
    {
         value=value+((int)(buff[i]-'0'))*power;
         power*=10;
    }
    if(is_positive)
    {
        if(buff[0]=='+')
        {
            *n=value;
            return OK;
        }
        value=value+((int)(buff[0]-'0'))*power;
        *n=value;
        return OK;
    }
    value*=-1;
    *n=value;
    return OK;
}

int readFlt(char *buff, int true_size, float *ans)
{
   
    if(true_size>28)return ERR; //if read string has more size than float can accept then return error
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

int main(void) {
    char f[40];
    for(int i=0;i<40;++i)f[i]='\0';
    scanf("%s",f);
    int s=0;
    for(;s<40;++s)if(f[s]=='\0')break;
    float ans;
    float r=readFlt(f,s,&ans);
    printf("%f is answer\n",ans);
    return 0;
}