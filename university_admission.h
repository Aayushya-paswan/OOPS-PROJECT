#include<bits/stdc++.h>
using namespace std;

class university{
    private:
        vector<Student> admitted_students;
    public:
        university();
        int closing, opening;
        int total_seats;
        int total_admitted = 0;
        void admit_student(Student&);

        
};

class Student{
    private:
        string name;
        int income;
        vector<string> preferred_branch;
        string contact_number;
        int roll;
        string gender;
        string category;
    public:
    
        int age;
        int percentage_12th;
        int jee_rank;
        Student(string, int, int, vector<string>, string, int, int, string, string);
        bool is_eligible(int, int);
        void assign_roll();
        void print_details();
        bool scholarship_eligible(int, string);
        ~Student();
};

class branch{
    private:
        string Branch;
        int seats_available;
        int closing, opening;
    public:
        branch(string, int, int, int);
        bool allocate_seat(Student&);
        ~branch();
};


class Hostel{
    private:
        
    public:
        void allocate_room(Student&) = 0;
};

class Boys_Hostel : public Hostel{
    private:
        int total_rooms;
        int rooms_allocated;
    public:
        void allocate_room(Student&);
        ~Boys_Hostel();
};

class Girls_Hostel : public Hostel{
    private:
        int total_rooms;
        int rooms_allocated;
    public:
        void allocate_room(Student&);
        ~Girls_Hostel();
};

class Admission_office{

    public:
        bool process_applications(Student s,university u);    // assign hostel and everything
        
};