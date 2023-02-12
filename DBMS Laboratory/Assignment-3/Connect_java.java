import java.sql.*;
import java.util.Scanner;

public class Connect_java {

  //Define variables for connection

  static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
  static final String DB_URL = "jdbc:mysql://localhost/20CS30012";
  static final String USER = "20CS30012";
  static final String PASS = "20CS30012";

    public static void main(String[] args) {
    Connection conn = null;
    Statement stmt = null;
    Scanner sc = new Scanner(System.in);
    
    //All the query statements are stored in strings
    String[] queries = {
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

    System.out.println("Welcome to Covid 19 Hospital Management System");
    System.out.println("The following are the queries supported");
    System.out.println("1) Names of all physicians who are trained in procedure name “bypass surgery” ");
    System.out.println("2) Names of all physicians affiliated with the department name “cardiology” and trained in “bypass surgery” ");
    System.out.println("3) Names of all the nurses who have ever been on call for room 123 ");
    System.out.println("4) Names and addresses of all patients who were prescribed the medication named “remdesivir” ");
    System.out.println("5) Name and insurance id of all patients who stayed in the “icu” room type for more than 15 days ");
    System.out.println("6) Names of all nurses who assisted in the procedure name “bypass surgery”");
    System.out.println("7) Name and position of all nurses who assisted in the procedure name “bypass surgery” along with the names of and the accompanying physicians ");
    System.out.println("8) Obtain the names of all physicians who have performed a medical procedure they have never been trained to perform ");
    System.out.println("9) Names of all physicians who have performed a medical procedure that they are trained to perform, but such that the procedure was done at a date (Undergoes.Date) after the physician's certification expired (Trained_In.CertificationExpires) ");
    System.out.println("10) Same as the previous query, but include the following information in the results: Physician name, name of procedure, date when the procedure was carried out, name of the patient the procedure was carried out on ");
    System.out.println("11) Names of all patients (also include, for each patient, the name of the patient's physician), such that all the following are true: \n-> The patient has been prescribed some medication by his/her physician \n-> The patient has undergone a procedure with a cost larger that 5000 \n-> The patient has had at least two appointment where the physician was affiliated with the cardiology department \n-> The patient's physician is not the head of any department ");
    System.out.println("12) Name and brand of the medication which has been prescribed to the highest number of patients ");
    System.out.println("13) Names of all physicians who are trained in procedure which is entered by you ");
    System.out.println("Press -1 to exit");

    
    try {

      //Connect to mysql server

      Class.forName(JDBC_DRIVER);
      conn = DriverManager.getConnection(DB_URL, USER, PASS);
      stmt = conn.createStatement();


      while (true) {
        System.out.print("\nEnter query Number of your choice : ");
        int q = sc.nextInt();
        if (q == -1) break;

        if (q != 13) {
            //Execute the query in the server and get result
          ResultSet rs = stmt.executeQuery(queries[q - 1]);
          switch (q) {
            case 1:
              System.out.println("\n---Name---");
              while (rs.next()) {
                System.out.println(rs.getString(1));        //Iterate over each row of the result and print it
              }
              break;
            case 2:
            System.out.println("\n---Name---");
            while (rs.next()) {
              System.out.println(rs.getString(1));
            }
              break;
            case 3:
            System.out.println("\n---Name---");
            while (rs.next()) {
              System.out.println(rs.getString(1));
            }
              break;
            case 4:
            System.out.println("\n---Name---     ---Address---");
            while (rs.next()) 
              System.out.println(rs.getString(1)+"     "+rs.getString(2));
              break;
            case 5:
            System.out.println("\n---Name---         ---Insurance Id---");
            while (rs.next()) 
              System.out.println(rs.getString(1)+"     "+rs.getString(2));
              break;
            case 6:
            System.out.println("\n---Name---");
            while (rs.next()) 
              System.out.println(rs.getString(1));
              break;
            case 7:
            System.out.println("\n---Nurse Name---     ---Position---     ---Physician---");
            while (rs.next()) 
              System.out.println(rs.getString(1)+"              "+rs.getString(2)+"          "+rs.getString(3));
              break;
            case 8:
            System.out.println("\n---Name---");
            while (rs.next()) 
              System.out.println(rs.getString(1));
              break;
            case 9:
            System.out.println("\n---Name---");
            while (rs.next()) 
              System.out.println(rs.getString(1));
              break;
            case 10:
            System.out.println("\n---Phsycican Name---    ---Procedure----  ---Date---   ---Patient Name---");
            while (rs.next()) 
              System.out.println(rs.getString(1)+"           "+rs.getString(2)+"     "+rs.getString(3)+"     "+rs.getString(3));
              break;
            case 11:
            System.out.println("\n---Patient Name---    ---Physician Name---");
            while (rs.next()) 
              System.out.println(rs.getString(1)+"     "+rs.getString(2));
              break;
            case 12:
            System.out.println("\n---Name---     ---Brand---");
            while (rs.next()) 
              System.out.println(rs.getString(1)+"         "+rs.getString(2));
              break;
          }
          
        } else {

            //For query 13

          System.out.print("Enter the name of the procedure : ");
          sc.nextLine();
          String p = sc.nextLine();
          String f = "select Ph.Name " +
                      "from Trained_In as T,Physician as Ph,Procedures as Pr " +
                      "where EmployeeID=T.Physician and " +
                      "( T.Treatment=Pr.Code and " +
                      "Pr.Name='" + p + "')";
          ResultSet rs = stmt.executeQuery(f);
          System.out.println("---Physician Name---");
           while(rs.next())
           System.out.println(rs.getString(1));
        }
      }
    } catch (SQLException se) {
      se.printStackTrace();
    } catch (Exception e) {
      e.printStackTrace();
    } finally {
      try {
        if (stmt != null) stmt.close();
      } catch (SQLException se2) {
      }
      try {
        if (conn != null) conn.close();
      } catch (SQLException se) {
        se.printStackTrace();
      }
    }
    System.out.println("Thanks for using our portal :)");
  }
}
