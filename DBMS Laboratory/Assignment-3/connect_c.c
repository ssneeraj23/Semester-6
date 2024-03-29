#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>

// Need mysql libraries and can be installed in ubuntu as
// sudo apt-get update
// sudo apt-get install libmysqlclient-dev
// compilation command : gcc -o connect_c connect_c.c $(mysql_config --cflags) $(mysql_config --libs)

//The below macros define the connection parameters

#define HOST "localhost"
#define USER "20CS30012"
#define PASSWORD "20CS30012"
#define DATABASE "20CS30012"

//All the query statements are stored in strings

char *queries[] = {
      "select Ph.Name from Trained_In as T,Physician as Ph,Procedures as Pr where EmployeeID=T.Physician and ( T.Treatment=Pr.Code and Pr.Name='bypass surgery')",
      "select Ph.Name from Trained_In as T,Physician as Ph,Procedures as Pr, Affiliated_with as A, Department as D where EmployeeID=T.Physician and T.Treatment=Pr.Code and EmployeeID=A.Physician  and A.Department=D.DepartmentID and D.Name='cardiology' and Pr.name='bypass surgery' ",
      "select N.Name from On_Call as O, Nurse as N, Block as B, Room as R where O.Nurse=N.EmployeeID and O.BlockFloor=B.Floor and O.BlockCode=B.Code and R.BlockFloor=B.Floor and R.BlockCode=B.Code and R.Number=123",
      "select Pat.Name,Pat.Address from Patient as Pat, Prescribes as Pre, Medication as M where Pre.Patient=Pat.SSN and Pre.Medication=M.Code and M.name='remdesivir' ",
      "select P.Name, P.InsuranceID from Patient as P, Stay as S, Room as R where S.Patient=P.SSN and S.Room=R.Number and R.Type='icu' and TIMESTAMPDIFF(DAY,S.Start,S.End)>15",
      "select N.Name from Nurse as N, Undergoes as U , Procedures as P where U.Procedures=P.Code and U.AssistingNurse=N.EmployeeID and P.Name='bypass surgery' ",
      "select N.Name as Nurse,N.Position,Ph.Name as Physician from Nurse as N, Undergoes as U , Procedures as P, Physician as Ph where U.Procedures=P.Code and U.AssistingNurse=N.EmployeeID and U.Physician=Ph.EmployeeID and P.Name='bypass surgery' ",
      "select Ph.Name  from Physician as Ph, Undergoes as U where Ph.EmployeeID=U.Physician and  not exists ( select * from Trained_In as T where T.Physician=U.Physician and T.Treatment=U.Procedures and T.CertificationDate < U.Date ) ",
      "select Ph.Name  from Physician as Ph, Undergoes as U, Trained_In as T where Ph.EmployeeID=U.Physician and  T.Physician=U.Physician and U.Procedures=T.Treatment and U.Date > T.CertificationExpires",
      "select Ph.Name as Physician, Pr.Name as Procedure_Name, DATE(U.Date) as Date, Patient.Name as Patient_Name from Physician as Ph, Undergoes as U, Trained_In as T, Procedures as Pr, Patient where Ph.EmployeeID=U.Physician and  U.Patient=Patient.SSN and Pr.Code=U.Procedures and T.Physician=U.Physician and U.Procedures=T.Treatment and U.Date > T.CertificationExpires",
      "select P.Name as Patient_Name,Ph.Name as Physician_Name from Patient as P, Physician as Ph where Ph.EmployeeID=P.PCP and exists ( select * from Prescribes as Pr where Pr.Physician=P.PCP and Pr.Patient=P.SSN) and exists ( select * from Undergoes as U, Procedures as Proc where U.Procedures=Proc.Code and U.Patient=P.SSN and Proc.Cost>5000 ) and not exists ( select * from Department as D where D.Head=P.PCP ) and  2>=( select count(*) from Appointment as A, Affiliated_with as Af, Department as D  where A.Patient=P.SSN and A.Physician=Af.Physician and Af.Department=D.DepartmentID and D.Name='cardiology' )",
      "select M.Name,M.Brand from Prescribes as P , Medication as M where P.Medication=M.Code group by M.Code having count(*)>=all( select count(*) from Prescribes as P , Medication as M where P.Medication=M.Code group by M.Code )"
    };

int main()
{

    //defining required variables for connection

    MYSQL *conn; 
    MYSQL_RES *result;
    MYSQL_ROW row;
    int q, num_fields;

    conn = mysql_init(NULL);

    //Connecting to mysql server
    if (!mysql_real_connect(conn, HOST, USER, PASSWORD, DATABASE, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    printf("Welcome to Covid 19 Hospital Management System\n");
    printf("The following are the queries supported");
    printf("\n");printf("1) Names of all physicians who are trained in procedure name “bypass surgery” ");
    printf("\n");printf("2) Names of all physicians affiliated with the department name “cardiology” and trained in “bypass surgery” ");
    printf("\n");printf("3) Names of all the nurses who have ever been on call for room 123 ");
    printf("\n");printf("4) Names and addresses of all patients who were prescribed the medication named “remdesivir” ");
    printf("\n");printf("5) Name and insurance id of all patients who stayed in the “icu” room type for more than 15 days ");
    printf("\n");printf("6) Names of all nurses who assisted in the procedure name “bypass surgery”");
    printf("\n");printf("7) Name and position of all nurses who assisted in the procedure name “bypass surgery” along with the names of and the accompanying physicians ");
    printf("\n");printf("8) Obtain the names of all physicians who have performed a medical procedure they have never been trained to perform ");
    printf("\n");printf("9) Names of all physicians who have performed a medical procedure that they are trained to perform, but such that the procedure was done at a date (Undergoes.Date) after the physician's certification expired (Trained_In.CertificationExpires) ");
    printf("\n");printf("10) Same as the previous query, but include the following information in the results: Physician name, name of procedure, date when the procedure was carried out, name of the patient the procedure was carried out on ");
    printf("\n");printf("11) Names of all patients (also include, for each patient, the name of the patient's physician), such that all the following are true: \n-> The patient has been prescribed some medication by his/her physician \n-> The patient has undergone a procedure with a cost larger that 5000 \n-> The patient has had at least two appointment where the physician was affiliated with the cardiology department \n-> The patient's physician is not the head of any department ");
    printf("\n");printf("12) Name and brand of the medication which has been prescribed to the highest number of patients ");
    printf("\n");printf("13) Names of all physicians who are trained in procedure which is entered by you ");
    printf("\n");printf("Press -1 to exit");
    while (1)
    {
        printf("\nEnter query number of your choice : ");
        scanf("%d", &q);
        if (q == -1)
            break;

        if (q != 13)
        {
            //Making a query to the server
            if (mysql_query(conn, queries[q - 1]))
            {
                fprintf(stderr, "%s\n", mysql_error(conn));
                continue;
            }
            //store the result of the query
            result = mysql_use_result(conn);
            
            //Print the result in proper format according to the queries

            switch (q)
            {
            case 1:
                printf("\n---Name---\n");
                while (row=mysql_fetch_row(result))        //Iterate over each row and print 
                {
                    printf("%s\n",row[0]);
                }
                break;
            case 2:
                printf("\n---Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s\n", row[0]);
                }
                break;
            case 3:
                printf("\n---Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s\n", row[0]);
                }
                break;
            case 4:
                printf("\n---Name---     ---Addresults---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s     %s\n", row[0], row[1]);
                }
                break;
            case 5:
                printf("\n---Name---         ---Insurance Id---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s     %s\n", row[0], row[1]);
                }
                break;
            case 6:
                printf("\n---Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s\n", row[0]);
                }
                break;
            case 7:
                printf("\n---Nurse Name---     ---Position---     ---Physician---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s              %s          %s\n", row[0], row[1], row[2]);
                }
                break;
            case 8:
                printf("\n---Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s\n", row[0]);
                }
                break;
            case 9:
                printf("\n---Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s\n", row[0]);
                }
                break;
            case 10:
                printf("\n---Phsycican Name---    ---Procedure----  ---Date---   ---Patient Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s           %s     %s     %s\n", row[0], row[1], row[2], row[3]);
                }
                break;
            case 11:
                printf("\n---Patient Name---    ---Physician Name---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s     %s\n", row[0], row[1]);
                }
                break;
            case 12:
                printf("\n---Name---     ---Brand---\n");
                while (row=mysql_fetch_row(result))
                {
                    printf("%s         %s\n", row[0], row[1]);
                }
                break;
            default:
                break;
            }
            mysql_free_result(result);
        }
        else
        {
            //For query 13
            char p[100];
            printf("Enter the name of the procedure: ");
            char k[2];
            fgets(k,2,stdin);
            fgets(p,100,stdin);
            for(int i=0;i<100;++i)
            {
                if(p[i]=='\n')
                {
                    p[i]='\0';
                    break;
                }
            }
            char f[500];
            sprintf(f, "select Ph.Name from Trained_In as T, Physician as Ph, Procedures as Pr where EmployeeID=T.Physician and (T.Treatment=Pr.Code and Pr.Name='%s');", p);
            if (mysql_query(conn, f))
            {
                fprintf(stderr, "%s\n", mysql_error(conn));
                continue;
            }

            result = mysql_use_result(conn);
            printf("\n---Name---\n");
            while (row=mysql_fetch_row(result))
            {
                printf("%s\n", row[0]);
            }
            mysql_free_result(result);
        }
    }

    mysql_close(conn);

    printf("Thanks for using our portal :)\n");
    return 0;
}
