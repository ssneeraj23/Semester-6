import mysql.connector
from tabulate import tabulate

#Make a conneciton to mysql server
mydb = mysql.connector.connect(
  host="localhost",
  user="20CS30012",
  password="20CS30012",
  database="20CS30012"
)
#storing headers in a dict correspoding to each query
headers = {
        1: ['Name'],
        2: ['Name'],
        3: ['Name'],
        4: ['Name', 'Address'],
        5: ['Name', 'Insurance_ID'],
        6: ['Name'],
        7: ['Nurse Name', 'Position', 'Phsycian Name'],
        8: ['Name'],
        9: ['Name'],
        10: ['Phsycican Name', 'Procedure', 'Date', 'Patient Name'],
        11: ['Patient Name', 'Physician Name'],
        12: ['Name', 'Brand'],
        13: ['Name']
    }

queries=['''select Ph.Name
from Trained_In as T,Physician as Ph,Procedures as Pr
where EmployeeID=T.Physician and
( T.Treatment=Pr.Code and
Pr.Name='bypass surgery');''',]

queries.append ('''select Ph.Name
from Trained_In as T,Physician as Ph,Procedures as Pr, Affiliated_with as A, Department as D
where EmployeeID=T.Physician and
T.Treatment=Pr.Code and
EmployeeID=A.Physician  and
A.Department=D.DepartmentID and
D.Name='cardiology' and
Pr.name='bypass surgery' ''')

queries.append('''select N.Name
from On_Call as O, Nurse as N, Block as B, Room as R
where O.Nurse=N.EmployeeID and
O.BlockFloor=B.Floor and
O.BlockCode=B.Code and
R.BlockFloor=B.Floor and
R.BlockCode=B.Code and
R.Number=123''')


queries.append('''select Pat.Name,Pat.Address
from Patient as Pat, Prescribes as Pre, Medication as M
where Pre.Patient=Pat.SSN and
Pre.Medication=M.Code and
M.name='remdesivir' ''')


queries.append('''select P.Name, P.InsuranceID
from Patient as P, Stay as S, Room as R
where S.Patient=P.SSN and
S.Room=R.Number and
R.Type='icu' and
TIMESTAMPDIFF(DAY,S.Start,S.End)>15 ''')


queries.append('''select N.Name
from Nurse as N, Undergoes as U , Procedures as P
where U.Procedures=P.Code and
U.AssistingNurse=N.EmployeeID and
P.Name='bypass surgery' ''')


queries.append(''' select N.Name as Nurse,N.Position,Ph.Name as Physician
from Nurse as N, Undergoes as U , Procedures as P, Physician as Ph
where U.Procedures=P.Code and
U.AssistingNurse=N.EmployeeID and
U.Physician=Ph.EmployeeID and
P.Name='bypass surgery' ''')


queries.append(''' select Ph.Name 
from Physician as Ph, Undergoes as U
where Ph.EmployeeID=U.Physician and 
not exists ( select * from Trained_In as T where T.Physician=U.Physician and T.Treatment=U.Procedures and T.CertificationDate < U.Date ) ''')


queries.append('''select Ph.Name 
from Physician as Ph, Undergoes as U, Trained_In as T
where Ph.EmployeeID=U.Physician and 
T.Physician=U.Physician and
U.Procedures=T.Treatment and
U.Date > T.CertificationExpires''')


queries.append('''select Ph.Name as Physician, Pr.Name as Procedure_Name, DATE(U.Date) as Date, Patient.Name as Patient_Name
from Physician as Ph, Undergoes as U, Trained_In as T, Procedures as Pr, Patient
where Ph.EmployeeID=U.Physician and 
U.Patient=Patient.SSN and
Pr.Code=U.Procedures and
T.Physician=U.Physician and
U.Procedures=T.Treatment and
U.Date > T.CertificationExpires''')


queries.append('''select P.Name as Patient_Name,Ph.Name as Physician_Name
from Patient as P, Physician as Ph
where Ph.EmployeeID=P.PCP and
exists ( select * from Prescribes as Pr where Pr.Physician=P.PCP and Pr.Patient=P.SSN) and
exists ( select * from Undergoes as U, Procedures as Proc where U.Procedures=Proc.Code and U.Patient=P.SSN and Proc.Cost>5000 ) and
not exists ( select * from Department as D where D.Head=P.PCP ) and 
2>=( select count(*) from Appointment as A, Affiliated_with as Af, Department as D 
         where A.Patient=P.SSN and
         A.Physician=Af.Physician and
         Af.Department=D.DepartmentID and
         D.Name='cardiology' )''')


queries.append('''select M.Name,M.Brand
from Prescribes as P , Medication as M
where P.Medication=M.Code
group by M.Code
having count(*)>=all(
select count(*)
from Prescribes as P , Medication as M
where P.Medication=M.Code
group by M.Code )''')



print("Welcome to Covid 19 Hospital Management System")
print("The following are the queries supported")
print("1) Names of all physicians who are trained in procedure name “bypass surgery” ")
print("2) Names of all physicians affiliated with the department name “cardiology” and trained in “bypass surgery” ")
print("3) Names of all the nurses who have ever been on call for room 123 ")
print("4) Names and addresses of all patients who were prescribed the medication named “remdesivir” ")
print("5) Name and insurance id of all patients who stayed in the “icu” room type for more than 15 days ")
print("6) Names of all nurses who assisted in the procedure name “bypass surgery”")
print("7) Name and position of all nurses who assisted in the procedure name “bypass surgery” along with the names of and the accompanying physicians ")
print("8) Obtain the names of all physicians who have performed a medical procedure they have never been trained to perform ")
print("9) Names of all physicians who have performed a medical procedure that they are trained to perform, but such that the procedure was done at a date (Undergoes.Date) after the physician's certification expired (Trained_In.CertificationExpires) ")
print("10) Same as the previous query, but include the following information in the results: Physician name, name of procedure, date when the procedure was carried out, name of the patient the procedure was carried out on ")
print("11) Names of all patients (also include, for each patient, the name of the patient's physician), such that all the following are true: \n-> The patient has been prescribed some medication by his/her physician \n-> The patient has undergone a procedure with a cost larger that 5000 \n-> The patient has had at least two appointment where the physician was affiliated with the cardiology department \n-> The patient's physician is not the head of any department ")
print("12) Name and brand of the medication which has been prescribed to the highest number of patients ")
print("13) Names of all physicians who are trained in procedure which is entered by you ")
print("Press -1 to exit")

mycursor = mydb.cursor()
while(1):
    q=input("\nEnter query Number of your choice : ")
    q=int(q)
    if(q==-1):break
    if(q!=13):
         mycursor.execute(queries[q-1])    #Execute query according the number
         myresult = mycursor.fetchall()    #get the result table
    else :
        p=input("Enter the name of the procedure : ") 
        f= '''select Ph.Name
                from Trained_In as T,Physician as Ph,Procedures as Pr
                where EmployeeID=T.Physician and
                ( T.Treatment=Pr.Code and
                Pr.Name='{}');'''
        f=f.format(p)       #For 13th query
        mycursor.execute(f)
        myresult = mycursor.fetchall()
    
    #using tabulate module print the result table with proper formatting  
    print(" ")
    print(tabulate(myresult, headers=headers.get(q, ""))) 

print("Thanks for using our portal :)")

