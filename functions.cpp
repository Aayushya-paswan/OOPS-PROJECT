#include "university_admission.h"
#include <iomanip>
#include <algorithm>

void printHeader(const string& title) {
    cout << "\n" << string(60, '=') << endl;
    cout << " " << title << endl;
    cout << string(60, '=') << endl;
}

void printSection(const string& title) {
    cout << "\n" << string(40, '-') << endl;
    cout << " " << title << endl;
    cout << string(40, '-') << endl;
}

int Student::next_roll = 1;

Student::Student(string name_,
                 int income_,
                 int jee_rank_,
                 vector<string> preferred_branches_,
                 string contact_number_,
                 int percentage_12th_,
                 int age_,
                 string gender_,
                 string category_) :
    roll_number(next_roll++),
    name(std::move(name_)),
    income(std::max(0, income_)),
    preferred_branches(std::move(preferred_branches_)),
    contact_number(std::move(contact_number_)),
    percentage_12th(std::max(0, std::min(100, percentage_12th_))),
    age(std::max(0, age_)),
    gender(std::move(gender_)),
    category(std::move(category_)),
    admitted(false),
    assigned_branch(""),
    hostel_allocated(false),
    mess_allocated(false),
    mess_plan(""),
    jee_rank(std::max(1, jee_rank_))
{
    if (find(CATEGORIES.begin(), CATEGORIES.end(), category) == CATEGORIES.end()) {
        category = "General";
    }

    transform(gender.begin(), gender.end(), gender.begin(), ::tolower);
    if (gender != "male" && gender != "female" && gender != "m" && gender != "f") {
        gender = "male";
    } else {
        if (gender == "m") gender = "male";
        if (gender == "f") gender = "female";
    }
}

bool Student::is_eligible(int min_percentage_12th, int min_age) const {
    return (percentage_12th >= min_percentage_12th) && (age >= min_age);
}

void Student::assign_roll() {}

void Student::set_admitted(const string& branch) {
    admitted = true;
    assigned_branch = branch;
}

bool Student::is_admitted() const { return admitted; }

const string& Student::get_name() const { return name; }
int Student::get_income() const { return income; }
const vector<string>& Student::get_preferences() const { return preferred_branches; }
const string& Student::get_gender() const { return gender; }
const string& Student::get_category() const { return category; }
int Student::get_roll() const { return roll_number; }
int Student::get_percentage_12th() const { return percentage_12th; }
int Student::get_age() const { return age; }

bool Student::scholarship_eligible(int income_threshold, const string& category_pref) const {
    return (income <= income_threshold) || (category == category_pref);
}

void Student::allocate_hostel() { hostel_allocated = true; }
bool Student::has_hostel() const { return hostel_allocated; }

void Student::allocate_mess(const string& plan) {
    mess_allocated = true;
    mess_plan = plan;
}
bool Student::has_mess() const { return mess_allocated; }
const string& Student::get_mess_plan() const { return mess_plan; }

void Student::print_details() const {
    cout << "| " << setw(4) << roll_number << " | "
         << setw(20) << left << name.substr(0, 19) << " | "
         << setw(8) << jee_rank << " | "
         << setw(6) << percentage_12th << "% | "
         << setw(4) << age << " | "
         << setw(6) << gender << " | "
         << setw(6) << category << " | "
         << setw(12) << (admitted ? assigned_branch : "Not Admitted") << " | "
         << setw(6) << (hostel_allocated ? "Yes" : "No") << " | "
         << setw(8) << (mess_allocated ? mess_plan : "No") << " |" << endl;
}

void Student::print_detailed_info() const {
    cout << "+-------------------------------------------------------------+" << endl;
    cout << "|                    STUDENT DETAILED INFO                    |" << endl;
    cout << "+-------------------------------------------------------------+" << endl;
    cout << "| Roll Number: " << setw(45) << left << roll_number << "|" << endl;
    cout << "| Name: " << setw(52) << left << name << "|" << endl;
    cout << "| JEE Rank: " << setw(48) << left << jee_rank << "|" << endl;
    cout << "| 12th Percentage: " << setw(40) << left << (to_string(percentage_12th) + "%") << "|" << endl;
    cout << "| Age: " << setw(53) << left << age << "|" << endl;
    cout << "| Gender: " << setw(50) << left << gender << "|" << endl;
    cout << "| Category: " << setw(48) << left << category << "|" << endl;
    cout << "| Contact: " << setw(49) << left << contact_number << "|" << endl;
    cout << "| Annual Income: " << setw(43) << left << ("Rs" + to_string(income)) << "|" << endl;

    string eligibility = is_eligible() ? "ELIGIBLE" : "NOT ELIGIBLE";
    if (!is_eligible()) {
        if (percentage_12th < 60)
            eligibility += " (12th% < 60%)";
        if (age < 17)
            eligibility += " (Age < 17)";
    }
    cout << "| Eligibility: " << setw(45) << left << eligibility << "|" << endl;

    cout << "| Admission: " << setw(47) << left
         << (admitted ? "ADMITTED - " + assigned_branch : "Not Admitted") << "|" << endl;

    cout << "| Branch Preferences: " << setw(38) << left;
    string prefs;
    for (size_t i = 0; i < preferred_branches.size(); ++i) {
        if (i > 0) prefs += ", ";
        prefs += preferred_branches[i];
    }
    cout << prefs << "|" << endl;

    cout << "| Hostel: " << setw(50) << left
         << (hostel_allocated ? "Allocated" : "Not Allocated") << "|" << endl;
    cout << "| Mess: " << setw(52) << left
         << (mess_allocated ? mess_plan + " Plan" : "Not Allocated") << "|" << endl;
    cout << "+-------------------------------------------------------------+" << endl;
}

Student::~Student() = default;

Mess::Mess(string name_, int capacity_) :
    name(std::move(name_)),
    capacity(capacity_),
    current_allocations(0)
{
    plan_prices = {{"Basic", 2000}, {"Standard", 3000}, {"Premium", 4500}};
}

bool Mess::allocate_mess(Student& s, const string& plan) {
    if (current_allocations >= capacity) {
        cout << "Mess is full! Cannot allocate for " << s.get_name() << endl;
        return false;
    }

    if (plan_prices.find(plan) == plan_prices.end()) {
        cout << "Invalid mess plan: " << plan << " for " << s.get_name() << endl;
        return false;
    }

    if (s.has_mess()) {
        cout << "Student " << s.get_name() << " already has mess allocation!" << endl;
        return false;
    }

    if (!s.has_hostel()) {
        cout << "Student " << s.get_name() << " must have hostel before mess allocation!" << endl;
        return false;
    }

    s.allocate_mess(plan);
    current_allocations++;
    cout << "Allocated " << plan << " mess plan to " << s.get_name()
         << " (Price: Rs" << get_plan_price(plan) << ")" << endl;
    return true;
}

bool Mess::is_available() const {
    return current_allocations < capacity;
}

void Mess::set_plan_price(const string& plan, int price) {
    plan_prices[plan] = price;
}

int Mess::get_plan_price(const string& plan) const {
    auto it = plan_prices.find(plan);
    return it != plan_prices.end() ? it->second : 0;
}

void Mess::print_status() const {
    cout << "+-------------------------------------------------------------+" << endl;
    cout << "| " << setw(58) << left << ("Mess: " + name) << "|" << endl;
    cout << "+-------------------------------------------------------------+" << endl;
    cout << "| Capacity: " << setw(4) << capacity
         << " | Allocated: " << setw(4) << current_allocations
         << " | Available: " << setw(4) << (capacity - current_allocations) << " |" << endl;

    cout << "| Plan Prices: ";
    for (const auto& plan : MESS_PLANS) {
        auto it = plan_prices.find(plan);
        if (it != plan_prices.end()) {
            cout << plan << ": Rs" << it->second << "  ";
        }
    }
    cout << "|" << endl;
    cout << "+-------------------------------------------------------------+" << endl;
}

Mess::~Mess() = default;

Branch::Branch(string name_, int general_seats_, unordered_map<string,int> reserved_,
               int opening_rank_, int closing_rank_) :
    name(std::move(name_)),
    general_seats(std::max(0, general_seats_)),
    reserved_seats(std::move(reserved_)),
    opening_rank(std::max(1, opening_rank_)),
    closing_rank(std::max(1, closing_rank_))
{
    if (closing_rank < opening_rank) {
        closing_rank = opening_rank;
    }
}

bool Branch::allocate_seat(const Student& s) {
    if (s.jee_rank < opening_rank || s.jee_rank > closing_rank) {
        return false;
    }

    auto it = reserved_seats.find(s.get_category());
    if (it != reserved_seats.end() && it->second > 0) {
        --(it->second);
        cout << "Allocated " << name << " seat to " << s.get_name()
             << " under " << s.get_category() << " category" << endl;
        return true;
    }

    if (general_seats > 0) {
        --general_seats;
        cout << "Allocated " << name << " seat to " << s.get_name()
             << " under General category" << endl;
        return true;
    }

    return false;
}

const string& Branch::get_name() const { return name; }

int Branch::seats_total() const {
    int sum = general_seats;
    for (const auto &p : reserved_seats) sum += p.second;
    return sum;
}

int Branch::general_remaining() const { return general_seats; }

int Branch::reserved_remaining(const string& category) const {
    auto it = reserved_seats.find(category);
    return it != reserved_seats.end() ? it->second : 0;
}

const unordered_map<string,int>& Branch::get_reserved_map() const { return reserved_seats; }

void Branch::print_seat_status() const {
    cout << "| " << setw(15) << left << name
         << "| General: " << setw(3) << general_seats;

    for (const auto& cat : CATEGORIES) {
        int rem = reserved_remaining(cat);
        if (rem > 0) {
            cout << "| " << cat << ": " << setw(3) << rem;
        }
    }
    cout << "| Rank Range: " << opening_rank << "-" << closing_rank << " |" << endl;
}

Branch::~Branch() = default;

Hostel::Hostel(string name_, int rooms) :
    name(std::move(name_)),
    total_rooms(std::max(0, rooms)),
    rooms_allocated(0)
{ }

bool Hostel::is_available() const {
    return rooms_allocated < total_rooms;
}

void Hostel::print_status() const {
    cout << "| " << setw(15) << left << name
         << "| Total: " << setw(4) << total_rooms
         << "| Allocated: " << setw(4) << rooms_allocated
         << "| Available: " << setw(4) << (total_rooms - rooms_allocated) << "|" << endl;
}

BoysHostel::BoysHostel(string name_, int rooms) : Hostel(name_, rooms) { }

void BoysHostel::allocate_room(Student& s) {
    string g = s.get_gender();
    if (is_available() && (g == "male")) {
        ++rooms_allocated;
        s.allocate_hostel();
        cout << "Allocated boys hostel room to " << s.get_name() << endl;
    }
}

BoysHostel::~BoysHostel() = default;

GirlsHostel::GirlsHostel(string name_, int rooms) : Hostel(name_, rooms) { }

void GirlsHostel::allocate_room(Student& s) {
    string g = s.get_gender();
    if (is_available() && (g == "female")) {
        ++rooms_allocated;
        s.allocate_hostel();
        cout << "Allocated girls hostel room to " << s.get_name() << endl;
    }
}

GirlsHostel::~GirlsHostel() = default;

int University::total_admitted_global = 0;

University::University(string name_, int opening_rank_, int closing_rank_) :
    name(std::move(name_)),
    total_seats(0),
    branches(),
    admitted_students(),
    opening_rank(std::max(1, opening_rank_)),
    closing_rank(std::max(1, closing_rank_))
{
    if (closing_rank < opening_rank) {
        closing_rank = opening_rank;
    }
}

void University::add_branch(unique_ptr<Branch> b) {
    if (!b) return;
    total_seats += b->seats_total();
    branches.emplace(b->get_name(), std::move(b));
}

bool University::can_admit(const Student& s) const {
    if (s.jee_rank < opening_rank || s.jee_rank > closing_rank) return false;
    return total_seats > 0;
}

bool University::admit_student(Student& s) {
    if (s.is_admitted()) return false;
    if (!s.is_eligible()) return false;
    if (!can_admit(s)) return false;

    for (const auto & pref : s.get_preferences()) {
        auto it = branches.find(pref);
        if (it == branches.end()) continue;
        Branch* br = it->second.get();
        if (!br) continue;
        if (br->allocate_seat(s)) {
            admitted_students.push_back(&s);
            s.set_admitted(pref);
            total_seats -= 1;
            ++total_admitted_global;
            return true;
        }
    }
    return false;
}

void University::print_admitted() const {
    cout << "+-------------------------------------------------------------+" << endl;
    cout << "| " << setw(58) << left << ("University: " + name) << "|" << endl;
    cout << "| Admitted Students: " << setw(38) << left << admitted_students.size() << "|" << endl;
    cout << "| Rank Range: " << setw(44) << left << (to_string(opening_rank) + " - " + to_string(closing_rank)) << "|" << endl;
    cout << "+-------------------------------------------------------------+" << endl;

    if (admitted_students.empty()) {
        cout << "| No students admitted                                              |" << endl;
    } else {
        for (const Student* p : admitted_students) {
            if (p) {
                string branch = p->is_admitted() ? p->assigned_branch : "Unknown";
                cout << "| > Roll " << setw(4) << p->get_roll()
                     << " | " << setw(20) << left << p->get_name().substr(0, 19)
                     << " | Rank: " << setw(6) << p->jee_rank
                     << " | Branch: " << setw(12) << left << branch << "|" << endl;
            }
        }
    }
    cout << "+-------------------------------------------------------------+" << endl;
}

void University::print_branch_status() const {
    cout << "+-------------------------------------------------------------+" << endl;
    cout << "| " << setw(58) << left << ("Branch Status - " + name) << "|" << endl;
    cout << "+-------------------------------------------------------------+" << endl;
    for (const auto& branch : branches) {
        if (branch.second) {
            branch.second->print_seat_status();
        }
    }
    cout << "+-------------------------------------------------------------+" << endl;
}

const unordered_map<string, unique_ptr<Branch>>& University::get_branches() const { return branches; }
const string& University::get_name() const { return name; }
int University::get_admitted_count() const { return admitted_students.size(); }

University::~University() = default;

AdmissionOffice::AdmissionOffice() :
    universities(),
    boys_hostel(nullptr),
    girls_hostel(nullptr),
    mess(nullptr)
{ }

void AdmissionOffice::add_university(unique_ptr<University> u) {
    if (!u) return;
    universities.push_back(std::move(u));
}

void AdmissionOffice::set_hostels(unique_ptr<BoysHostel> b, unique_ptr<GirlsHostel> g) {
    boys_hostel = std::move(b);
    girls_hostel = std::move(g);
}

void AdmissionOffice::set_mess(unique_ptr<Mess> m) {
    mess = std::move(m);
}

void AdmissionOffice::process_applications(vector<Student>& applicants) {
    sort(applicants.begin(), applicants.end(), [](const Student& a, const Student& b){
        return a.jee_rank < b.jee_rank;
    });

    cout << "\nProcessing " << applicants.size() << " applications..." << endl;

    for (auto & student : applicants) {
        if (!student.is_eligible()) {
            cout << "Skipping " << student.get_name() << " - Not eligible" << endl;
            continue;
        }

        bool admitted = false;
        for (auto & uni_ptr : universities) {
            if (!uni_ptr) continue;

            if (uni_ptr->admit_student(student)) {
                admitted = true;
                cout << "Admitted " << student.get_name() << " to "
                     << uni_ptr->get_name() << endl;

                string g = student.get_gender();

                if (g == "male" && boys_hostel) {
                    boys_hostel->allocate_room(student);
                } else if (g == "female" && girls_hostel) {
                    girls_hostel->allocate_room(student);
                }

                if (student.has_hostel() && mess) {
                    string mess_plan = "Standard";

                    if (student.get_income() <= 100000 ||
                        student.get_category() == "SC" ||
                        student.get_category() == "ST") {
                        mess_plan = "Basic";
                    }
                    else if (student.get_income() > 500000) {
                        mess_plan = "Premium";
                    }

                    mess->allocate_mess(student, mess_plan);
                }
                break;
            }
        }

        if (!admitted) {
            cout << "Could not admit " << student.get_name() << " - No suitable branch/university" << endl;
        }
    }

    cout << "Application processing completed." << endl;
}

void AdmissionOffice::show_all_admissions() const {
    printHeader("ADMISSIONS SUMMARY");

    int total_admitted = 0;
    for (const auto & uni_ptr : universities) {
        if (uni_ptr) {
            total_admitted += uni_ptr->get_admitted_count();
            uni_ptr->print_admitted();
        }
    }

    cout << "\nTOTAL STUDENTS ADMITTED: " << total_admitted << endl;
}

void AdmissionOffice::show_detailed_report() const {
    printHeader("DETAILED ADMISSION REPORT");

    for (const auto & uni_ptr : universities) {
        if (uni_ptr) {
            uni_ptr->print_branch_status();
        }
    }

    printSection("HOSTEL STATUS");
    if (boys_hostel) boys_hostel->print_status();
    if (girls_hostel) girls_hostel->print_status();

    if (mess) {
        printSection("MESS STATUS");
        mess->print_status();
    }
}

void AdmissionOffice::show_eligibility_report(const vector<Student>& applicants) const {
    printHeader("ELIGIBILITY REPORT");

    cout << "+-------+----------------------+--------+--------+------+--------+----------+-------------------+" << endl;
    cout << "| Roll  | Name                 | JEE    | 12th%  | Age  | Gender | Category | Status           |" << endl;
    cout << "+-------+----------------------+--------+--------+------+--------+----------+-------------------+" << endl;

    for (const auto& student : applicants) {
        cout << "| " << setw(5) << student.get_roll() << " | "
             << setw(20) << left << student.get_name().substr(0, 19) << " | "
             << setw(6) << student.jee_rank << " | "
             << setw(6) << student.get_percentage_12th() << " | "
             << setw(4) << student.get_age() << " | "
             << setw(6) << student.get_gender() << " | "
             << setw(8) << student.get_category() << " | ";

        if (!student.is_eligible()) {
            cout << "NOT ELIGIBLE ";
            if (student.get_percentage_12th() < 60) cout << "12th% < 60";
            if (student.get_percentage_12th() < 60 && student.get_age() < 17) cout << ", ";
            if (student.get_age() < 17) cout << "Age < 17";
        } else if (!student.is_admitted()) {
            cout << "Not Admitted   ";
        } else {
            cout << "Admitted       ";
        }
        cout << setw(4) << " |" << endl;
    }
    cout << "+-------+----------------------+--------+--------+------+--------+----------+-------------------+" << endl;
}

const vector<unique_ptr<University>>& AdmissionOffice::get_universities() const {
    return universities;
}

AdmissionOffice::~AdmissionOffice() = default;