#include<bits/stdc++.h>
using namespace std;

class university{
    private:
        int total_seats;
        vector<Student> admitted_students;
        static int total_admitted;
        int closing, opening;
    public:
        bool can_admit(Student&);
        void admit_student(Student&);

        ~university();
};

class Student{
    private:
        string name;
        int income;
        vector<string> preferred_branch;
        string contact_number;
        int percentage_12th;
        int age;
        string gender;
        string category;
    public:
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
        allocate_room(Student&) = 0;
};

class Boys_Hostel : public Hostel{
    private:
        int total_rooms;
        int rooms_allocated;
    public:
        allocate_room(Student&);
        ~Boys_Hostel();
};

class Girls_Hostel : public Hostel{
    private:
        int total_rooms;
        int rooms_allocated;
    public:
        allocate_room(Student&);
        ~Girls_Hostel();
};

class Admssion_office{
    private:
        vector<university> universities;
    public:
        void process_applications(vector<Student>&);    // assign hostel and everything
        ~Admssion_office();
};