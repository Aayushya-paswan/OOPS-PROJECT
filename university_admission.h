#ifndef UNIVERSITY_ADMISSION_H
#define UNIVERSITY_ADMISSION_H

#include <bits/stdc++.h>
using namespace std;

class Student;
class Branch;
class Hostel;
class Mess;
class University;
class AdmissionOffice;

static const vector<string> CATEGORIES = {"General","OBC","SC","ST","EWS"};
static const vector<string> MESS_PLANS = {"Basic", "Standard", "Premium"};

class Student {
private:
    static int next_roll;
    int roll_number;
    string name;
    int income;
    vector<string> preferred_branches;
    string contact_number;
    int age;
    string gender;
    string category;
    bool admitted;
    bool hostel_allocated;
    bool mess_allocated;
    string mess_plan;

public:
    string assigned_branch;
    int percentage_12th;
    int jee_rank;

    Student(string, int, int, vector<string>, string, int, int, string, string);
    bool is_eligible(int = 60, int = 17) const;
    void assign_roll();
    int get_percentage_12th() const;
    void set_admitted(const string&);
    bool is_admitted() const;
    const string& get_name() const;
    int get_income() const;
    const vector<string>& get_preferences() const;
    const string& get_gender() const;
    const string& get_category() const;
    int get_roll() const;
    int get_age() const;
    bool scholarship_eligible(int, const string&) const;
    void allocate_hostel();
    bool has_hostel() const;
    void allocate_mess(const string& plan = "Standard");
    bool has_mess() const;
    const string& get_mess_plan() const;
    void print_details() const;
    void print_detailed_info() const;
    ~Student();
};

class Mess {
private:
    string name;
    int capacity;
    int current_allocations;
    unordered_map<string, int> plan_prices;

public:
    Mess(string name_, int capacity_);
    bool allocate_mess(Student& s, const string& plan = "Standard");
    bool is_available() const;
    void set_plan_price(const string& plan, int price);
    int get_plan_price(const string& plan) const;
    void print_status() const;
    ~Mess();
};

class Branch {
private:
    string name;
    int general_seats;
    unordered_map<string,int> reserved_seats;
    int opening_rank;
    int closing_rank;

public:
    Branch(string name_, int general_seats_, unordered_map<string,int> reserved_ = {},
           int opening_rank_ = 1, int closing_rank_ = INT_MAX);
    virtual bool allocate_seat(const Student&);
    const string& get_name() const;
    int seats_total() const;
    int general_remaining() const;
    int reserved_remaining(const string& category) const;
    const unordered_map<string,int>& get_reserved_map() const;
    void print_seat_status() const;
    virtual ~Branch();
};

class Hostel {
protected:
    string name;
    int total_rooms;
    int rooms_allocated;

public:
    Hostel(string name_, int rooms);
    virtual void allocate_room(Student&) = 0;
    bool is_available() const;
    void print_status() const;
    virtual ~Hostel() = default;
};

class BoysHostel : public Hostel {
public:
    BoysHostel(string name_, int rooms);
    void allocate_room(Student&) override;
    ~BoysHostel() override;
};

class GirlsHostel : public Hostel {
public:
    GirlsHostel(string name_, int rooms);
    void allocate_room(Student&) override;
    ~GirlsHostel() override;
};

class University {
private:
    string name;
    int total_seats;
    unordered_map<string, unique_ptr<Branch>> branches;
    vector<const Student*> admitted_students;
    int opening_rank;
    int closing_rank;
    static int total_admitted_global;

public:
    University(string, int = 1, int = INT_MAX);
    void add_branch(unique_ptr<Branch>);
    bool can_admit(const Student&) const;
    bool admit_student(Student&);
    void print_admitted() const;
    void print_branch_status() const;
    const unordered_map<string, unique_ptr<Branch>>& get_branches() const;
    const string& get_name() const;
    int get_admitted_count() const;
    ~University();
};

class AdmissionOffice {
private:
    vector<unique_ptr<University>> universities;
    unique_ptr<BoysHostel> boys_hostel;
    unique_ptr<GirlsHostel> girls_hostel;
    unique_ptr<Mess> mess;

public:
    AdmissionOffice();
    void add_university(unique_ptr<University>);
    void set_hostels(unique_ptr<BoysHostel>, unique_ptr<GirlsHostel>);
    void set_mess(unique_ptr<Mess>);
    void process_applications(vector<Student>&);
    void show_all_admissions() const;
    void show_detailed_report() const;
    void show_eligibility_report(const vector<Student>& applicants) const;
    const vector<unique_ptr<University>>& get_universities() const;
    ~AdmissionOffice();
};

#endif