CREATE TABLE IF NOT EXISTS Physician (
    EmployeeID INT not NULL,
    Name text not NULL,
    Position text not NULL,
    SSN INT not NULL,
    primary key (EmployeeID)
);
CREATE TABLE IF NOT EXISTS Department(
    DepartmentID INT not NULL,
    Name text not NULL,
    Head INT not NULL,
    primary key (DepartmentID),
    FOREIGN KEY (Head) references Physician(EmployeeID) 
);
CREATE TABLE IF NOT EXISTS Affiliated_with(
    Physician INT not NULL,
    Department INT not NULL,
    PrimaryAffiliation BOOLEAN not NULL,
    primary key (Physician,Department),
    FOREIGN key (Physician) references Physician(EmployeeID),
    FOREIGN key (Department) references Department(DepartmentID)
);
CREATE TABLE IF NOT EXISTS Procedures(
    Code INT not NULL,
    Name text not NULL,
    Cost INT not NULL,
    primary key(Code)
);
CREATE TABLE IF NOT EXISTS Trained_In(
    Physician INT not NULL,
    Treatment INT not NULL,
    CertificationDate datetime not NULL,
    CertificationExpires datetime not NULL,
    primary key (Physician,Treatment),
    FOREIGN key (Physician) references Physician(EmployeeID),
    FOREIGN key (Treatment) references Procedures(Code)
    
);
CREATE TABLE IF NOT EXISTS Nurse(
    EmployeeID INT not NULL,
    Name text not NULL,
    Position text not NULL,
    Registered BOOLEAN not NULL,
    SSN INT not NULL,
    primary key (EmployeeID)
);
CREATE TABLE IF NOT EXISTS Patient(
    SSN INT not NULL,
    Name text not NULL,
    Address text not NULL,
    Phone text not NULL,
    InsuranceID INT not NULL,
    PCP INT not NULL,
    primary key (SSN),
    FOREIGN key (PCP) references Physician(EmployeeID)
);
CREATE TABLE Block(
    Floor INT not NULL,
    Code INT not NULL,
    primary key (Floor,Code)
);
CREATE TABLE Medication(
    Code INT not NULL,
    Name text not NULL,
    Brand text not NULL,
    Description text not NULL,
    primary key(Code)
);
CREATE TABLE IF NOT EXISTS Appointment(
    AppointmentID INT not NULL,
    Patient INT not NULL,
    PrepNurse INT,
    Physician INT not NULL,
    Start datetime not NULL,
    End datetime not NULL,
    ExaminationRoom text not NULL,
    primary key(AppointmentID),
    FOREIGN key (Physician) references Physician(EmployeeID),
    FOREIGN KEY (Patient) references Patient(SSN),
    FOREIGN key(PrepNurse) references Nurse(EmployeeID)
);
CREATE TABLE IF NOT EXISTS Prescribes(
    Physician INT not NULL,
    Patient INT not NULL,
    Medication INT not NULL,
    Date datetime not NULL,
    Appointment INT ,
    Dose text not NULL,
    primary key (Physician,Patient,Medication,Date),
    FOREIGN key (Physician) references Physician(EmployeeID),
    FOREIGN key (Patient) references Patient(SSN),
    FOREIGN KEY (Appointment) references Appointment(AppointmentID),
    FOREIGN KEY (Medication) references Medication(Code)
);
CREATE TABLE IF NOT EXISTS Room(
    Number INT not NULL,
    Type text not NULL,
    BlockFloor INT not NULL,
    BlockCode INT not NULL,
    Unavailable BOOLEAN not NULL,
    primary key(Number),
    FOREIGN key (BlockFloor,BlockCode) references Block(Floor,Code)
);
CREATE TABLE IF NOT EXISTS Stay (
    StayID INT not NULL,
    Patient INT not NULL,
    Room INT not NULL,
    Start datetime not NULL,
    End datetime not NULL,
    primary key (StayID),
    FOREIGN key (Room) references Room(Number),
    FOREIGN KEY (Patient) references Patient(SSN)
);
CREATE TABLE IF NOT EXISTS Undergoes(
    Patient INT not NULL,
    Procedures INT not NULL,
    Stay INT not NULL,
    Date datetime not NULL,
    Physician INT not NULL,
    AssistingNurse INT,
    primary key(Patient,Procedures,Stay,Date),
    FOREIGN key (Procedures) references Procedures(Code),
    FOREIGN key(AssistingNurse) references Nurse(EmployeeID),
    FOREIGN key (Physician) references Physician(EmployeeID),
    FOREIGN key (Patient) references Patient(SSN),
    FOREIGN key (Stay) references Stay(StayID)
);

CREATE TABLE IF NOT EXISTS On_Call(
    Nurse INT not NULL,
    BlockFloor INT not NULL,
    BlockCode INT not NULL,
    Start datetime not NULL,
    End datetime not NULL,
    primary key (Nurse,BlockFloor,BlockCode),
    FOREIGN key (BlockFloor,BlockCode) references Block(Floor,Code),
    FOREIGN key(Nurse) references Nurse(EmployeeID)
);


INSERT INTO Physician VALUES(1,'Alan Donald','Intern',111111111);
INSERT INTO Physician VALUES(2,'Bruce Reid','Attending Physician',222222222);
INSERT INTO Physician VALUES(3,'Courtney Walsh','Surgeon Physician',333333333);
INSERT INTO Physician VALUES(4,'Malcom Marshall','Senior Physician',444444444);
INSERT INTO Physician VALUES(5,'Dennis Lillee','Head Chief of Medicine',555555555);
INSERT INTO Physician VALUES(6,'Jeff Thomson','Surgeon Physician',666666666);
INSERT INTO Physician VALUES(7,'Richard Hadlee','Surgeon Physician',777777777);
INSERT INTO Physician VALUES(8,'Kapil  Dev','Resident',888888888);
INSERT INTO Physician VALUES(9,'Ishant Sharma','Psychiatrist',999999999);

INSERT INTO Department VALUES(1,'medicine',4);
INSERT INTO Department VALUES(2,'surgery',7);
INSERT INTO Department VALUES(3,'psychiatry',9);
INSERT INTO Department VALUES(4,'cardiology',8);

INSERT INTO Affiliated_with VALUES(1,1,1);
INSERT INTO Affiliated_with VALUES(2,1,1);
INSERT INTO Affiliated_with VALUES(3,1,0);
INSERT INTO Affiliated_with VALUES(3,2,1);
INSERT INTO Affiliated_with VALUES(4,1,1);
INSERT INTO Affiliated_with VALUES(5,1,1);
INSERT INTO Affiliated_with VALUES(6,2,1);
INSERT INTO Affiliated_with VALUES(7,1,0);
INSERT INTO Affiliated_with VALUES(7,2,1);
INSERT INTO Affiliated_with VALUES(8,1,1);
INSERT INTO Affiliated_with VALUES(9,3,1);
INSERT INTO Affiliated_with VALUES(3,4,1);

INSERT INTO Procedures VALUES(1,'bypass surgery',1500.0);
INSERT INTO Procedures VALUES(2,'angioplasty',3750.0);
INSERT INTO Procedures VALUES(3,'arthoscopy',4500.0);
INSERT INTO Procedures VALUES(4,'carotid endarterectomy',10000.0);
INSERT INTO Procedures VALUES(5,'cholecystectomy',4899.0);
INSERT INTO Procedures VALUES(6,'tonsillectomy',5600.0);
INSERT INTO Procedures VALUES(7,'cataract surgery',25.0);

INSERT INTO Patient VALUES(100000001,'Dilip Vengsarkar','42 Foobar Lane','555-0256',68476213,1);
INSERT INTO Patient VALUES(100000002,'Richie Richardson','37 Infinite Loop','555-0512',36546321,2);
INSERT INTO Patient VALUES(100000003,'Mark Waugh','101 Parkway Street','555-1204',65465421,2);
INSERT INTO Patient VALUES(100000004,'Ramiz Raza','1100 Sparks Avenue','555-2048',68421879,3);

INSERT INTO Nurse VALUES(101,'Eknath Solkar','Head Nurse',1,111111110);
INSERT INTO Nurse VALUES(102,'David Boon','Nurse',1,222222220);
INSERT INTO Nurse VALUES(103,'Andy Flowers','Nurse',0,333333330);

INSERT INTO Appointment VALUES(13216584,100000001,101,1,'2018-04-24 10:00','2018-04-24 11:00','A');
INSERT INTO Appointment VALUES(26548913,100000002,101,2,'2018-04-24 10:00','2018-04-24 11:00','B');
INSERT INTO Appointment VALUES(36549879,100000001,102,1,'2018-04-25 10:00','2018-04-25 11:00','A');
INSERT INTO Appointment VALUES(46846589,100000004,103,4,'2018-04-25 10:00','2018-04-25 11:00','B');
INSERT INTO Appointment VALUES(59871321,100000004,NULL,4,'2018-04-26 10:00','2018-04-26 11:00','C');
INSERT INTO Appointment VALUES(69879231,100000003,103,2,'2018-04-26 11:00','2018-04-26 12:00','C');
INSERT INTO Appointment VALUES(76983231,100000001,NULL,3,'2018-04-26 12:00','2018-04-26 13:00','C');
INSERT INTO Appointment VALUES(86213939,100000004,102,9,'2018-04-27 10:00','2018-04-21 11:00','A');
INSERT INTO Appointment VALUES(93216548,100000002,101,2,'2018-04-27 10:00','2018-04-27 11:00','B');
INSERT INTO Appointment VALUES(76983237,100000001,NULL,3,'2019-04-26 12:00','2019-04-26 13:00','C');

INSERT INTO Medication VALUES(1,'Paracetamol','Z','N/A');
INSERT INTO Medication VALUES(2,'Actemra','Foolki Labs','N/A');
INSERT INTO Medication VALUES(3,'Molnupiravir','Bale Laboratories','N/A');
INSERT INTO Medication VALUES(4,'Paxlovid','Bar Industries','N/A');
INSERT INTO Medication VALUES(5,'remdesivir','Donald Pharmaceuticals','N/A');

INSERT INTO Prescribes VALUES(1,100000001,1,'2018-04-24 10:47',13216584,'5');
INSERT INTO Prescribes VALUES(9,100000004,2,'2018-04-27 10:53',86213939,'10');
INSERT INTO Prescribes VALUES(9,100000004,5,'2018-04-27 10:53',86213939,'10');
INSERT INTO Prescribes VALUES(9,100000004,2,'2018-04-30 16:53',NULL,'5');

INSERT INTO Block VALUES(1,1);
INSERT INTO Block VALUES(1,2);
INSERT INTO Block VALUES(1,3);
INSERT INTO Block VALUES(2,1);
INSERT INTO Block VALUES(2,2);
INSERT INTO Block VALUES(2,3);
INSERT INTO Block VALUES(3,1);
INSERT INTO Block VALUES(3,2);
INSERT INTO Block VALUES(3,3);
INSERT INTO Block VALUES(4,1);
INSERT INTO Block VALUES(4,2);
INSERT INTO Block VALUES(4,3);

INSERT INTO Room VALUES(101,'icu',1,1,0);
INSERT INTO Room VALUES(102,'Single',1,1,0);
INSERT INTO Room VALUES(103,'Single',1,1,0);
INSERT INTO Room VALUES(111,'Single',1,2,0);
INSERT INTO Room VALUES(112,'Single',1,2,1);
INSERT INTO Room VALUES(113,'Single',1,2,0);
INSERT INTO Room VALUES(121,'Single',1,3,0);
INSERT INTO Room VALUES(122,'Single',1,3,0);
INSERT INTO Room VALUES(123,'Single',1,3,0);
INSERT INTO Room VALUES(201,'Single',2,1,1);
INSERT INTO Room VALUES(202,'Single',2,1,0);
INSERT INTO Room VALUES(203,'Single',2,1,0);
INSERT INTO Room VALUES(211,'Single',2,2,0);
INSERT INTO Room VALUES(212,'Single',2,2,0);
INSERT INTO Room VALUES(213,'Single',2,2,1);
INSERT INTO Room VALUES(221,'Single',2,3,0);
INSERT INTO Room VALUES(222,'Single',2,3,0);
INSERT INTO Room VALUES(223,'Single',2,3,0);
INSERT INTO Room VALUES(301,'Single',3,1,0);
INSERT INTO Room VALUES(302,'Single',3,1,1);
INSERT INTO Room VALUES(303,'Single',3,1,0);
INSERT INTO Room VALUES(311,'Single',3,2,0);
INSERT INTO Room VALUES(312,'Single',3,2,0);
INSERT INTO Room VALUES(313,'Single',3,2,0);
INSERT INTO Room VALUES(321,'Single',3,3,1);
INSERT INTO Room VALUES(322,'Single',3,3,0);
INSERT INTO Room VALUES(323,'Single',3,3,0);
INSERT INTO Room VALUES(401,'Single',4,1,0);
INSERT INTO Room VALUES(402,'Single',4,1,1);
INSERT INTO Room VALUES(403,'Single',4,1,0);
INSERT INTO Room VALUES(411,'Single',4,2,0);
INSERT INTO Room VALUES(412,'Single',4,2,0);
INSERT INTO Room VALUES(413,'Single',4,2,0);
INSERT INTO Room VALUES(421,'Single',4,3,1);
INSERT INTO Room VALUES(422,'Single',4,3,0);
INSERT INTO Room VALUES(423,'Single',4,3,0);

INSERT INTO On_Call VALUES(101,1,1,'2018-11-04 11:00','2018-11-04 19:00');
INSERT INTO On_Call VALUES(101,1,2,'2018-11-04 11:00','2018-11-04 19:00');
INSERT INTO On_Call VALUES(102,1,3,'2018-11-04 11:00','2018-11-04 19:00');
INSERT INTO On_Call VALUES(103,1,1,'2018-11-04 19:00','2018-11-05 03:00');
INSERT INTO On_Call VALUES(103,1,2,'2018-11-04 19:00','2018-11-05 03:00');
INSERT INTO On_Call VALUES(103,1,3,'2018-11-04 19:00','2018-11-05 03:00');

INSERT INTO Stay VALUES(3215,100000001,101,'2018-05-01','2018-06-04');
INSERT INTO Stay VALUES(3216,100000003,123,'2018-05-03','2018-05-14');
INSERT INTO Stay VALUES(3217,100000004,112,'2018-05-02','2018-05-03');

INSERT INTO Undergoes VALUES(100000001,6,3215,'2018-05-02',3,101);
INSERT INTO Undergoes VALUES(100000001,2,3215,'2018-05-03',7,101);
INSERT INTO Undergoes VALUES(100000004,1,3217,'2018-05-07',3,102);
INSERT INTO Undergoes VALUES(100000004,5,3217,'2018-05-09',6,NULL);
INSERT INTO Undergoes VALUES(100000001,7,3217,'2018-05-10',7,101);
INSERT INTO Undergoes VALUES(100000004,4,3217,'2018-05-13',3,103);

INSERT INTO Trained_In VALUES(3,1,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,2,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,5,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,6,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(3,7,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(6,2,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(6,5,'2017-01-01','2017-12-31');
INSERT INTO Trained_In VALUES(6,6,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,1,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,2,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,3,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,4,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,5,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,6,'2018-01-01','2018-12-31');
INSERT INTO Trained_In VALUES(7,7,'2018-01-01','2018-12-31');

select Ph.Name
from Trained_In as T,Physician as Ph,Procedures as Pr
where EmployeeID=T.Physician and
( T.Treatment=Pr.Code and
Pr.Name='bypass surgery');


select Ph.Name
from Trained_In as T,Physician as Ph,Procedures as Pr, Affiliated_with as A, Department as D
where EmployeeID=T.Physician and
T.Treatment=Pr.Code and
EmployeeID=A.Physician  and
A.Department=D.DepartmentID and
D.Name='cardiology' and
Pr.name='bypass surgery';

select N.Name
from On_Call as O, Nurse as N, Block as B, Room as R
where O.Nurse=N.EmployeeID and
O.BlockFloor=B.Floor and
O.BlockCode=B.Code and
R.BlockFloor=B.Floor and
R.BlockCode=B.Code and
R.Number=123;

select Pat.Name,Pat.Address
from Patient as Pat, Prescribes as Pre, Medication as M
where Pre.Patient=Pat.SSN and
Pre.Medication=M.Code and
M.name='remdesivir';

select P.Name, P.InsuranceID
from Patient as P, Stay as S, Room as R
where S.Patient=P.SSN and
S.Room=R.Number and
R.Type='icu' and
TIMESTAMPDIFF(DAY,S.Start,S.End)>15;

/*this is q6*/

select N.Name
from Nurse as N, Undergoes as U , Procedures as P
where U.Procedures=P.Code and
U.AssistingNurse=N.EmployeeID and
P.Name='bypass surgery';

select N.Name as Nurse,N.Position,Ph.Name as Physician
from Nurse as N, Undergoes as U , Procedures as P, Physician as Ph
where U.Procedures=P.Code and
U.AssistingNurse=N.EmployeeID and
U.Physician=Ph.EmployeeID and
P.Name='bypass surgery';

select Ph.Name 
from Physician as Ph, Undergoes as U
where Ph.EmployeeID=U.Physician and 
not exists ( select * from Trained_In as T where T.Physician=U.Physician and T.Treatment=U.Procedures and T.CertificationDate < U.Date );

select Ph.Name 
from Physician as Ph, Undergoes as U, Trained_In as T
where Ph.EmployeeID=U.Physician and 
T.Physician=U.Physician and
U.Procedures=T.Treatment and
U.Date > T.CertificationExpires;

select Ph.Name as Physician, Pr.Name as Procedure_Name, DATE(U.Date) as Date, Patient.Name as Patient_Name
from Physician as Ph, Undergoes as U, Trained_In as T, Procedures as Pr, Patient
where Ph.EmployeeID=U.Physician and 
U.Patient=Patient.SSN and
Pr.Code=U.Procedures and
T.Physician=U.Physician and
U.Procedures=T.Treatment and
U.Date > T.CertificationExpires;

select P.Name as Patient_Name,Ph.Name as Physician_Name
from Patient as P, Physician as Ph
where Ph.EmployeeID=P.PCP and
exists ( select * from Prescribes as Pr where Pr.Physician=P.PCP and Pr.Patient=P.SSN) and
exists ( select * from Undergoes as U, Procedures as Proc where U.Procedures=Proc.Code and U.Patient=P.SSN and Proc.Cost>5000 ) and
not exists ( select * from Department as D where D.Head=P.PCP ) and 
2<=( select count(*) from Appointment as A, Affiliated_with as Af, Department as D 
         where A.Patient=P.SSN and
         A.Physician=Af.Physician and
         Af.Department=D.DepartmentID and
         D.Name='cardiology' );

select M.Name,M.Brand
from Prescribes as P , Medication as M
where P.Medication=M.Code
group by M.Code
having count(*)>=all(
select count(*)
from Prescribes as P , Medication as M
where P.Medication=M.Code
group by M.Code );
