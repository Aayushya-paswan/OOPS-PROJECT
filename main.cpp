#include "functions.cpp"
#include <thread>
#include <sstream>
#include <cctype>
#include <fstream>

using namespace std::chrono_literals;
using namespace std;

static void pause_short() { std::this_thread::sleep_for(250ms); }
static void pause_medium() { std::this_thread::sleep_for(500ms); }

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static vector<string> split_preferences(const string &line) {
    vector<string> out;
    string token;
    istringstream iss(line);
    while (getline(iss, token, ',')) {
        token = trim(token);
        if (!token.empty()) out.push_back(token);
    }
    return out;
}

struct BranchInfo {
    string name;
    int general;
    unordered_map<string,int> reserved;
    int opening_rank;
    int closing_rank;
};

struct UnivInfo {
    string name;
    int opening_rank;
    int closing_rank;
    vector<BranchInfo> branches;
};

static void print_reserved_map(const unordered_map<string,int>& m) {
    cout << "| ";
    for (auto &cat : CATEGORIES) {
        auto it = m.find(cat);
        int val = (it == m.end() ? 0 : it->second);
        if (val > 0) {
            cout << cat << ": " << val << "  ";
        }
    }
}

void displayWelcomeMessage() {
    cout << "\n" << string(70, '=') << endl;
    cout << "    UNIVERSITY ADMISSION MANAGEMENT SYSTEM" << endl;
    cout << string(70, '=') << endl;
    cout << "Features:" << endl;
    cout << "    Multi-University Admission Processing" << endl;
    cout << "    Reservation-based Seat Allocation" << endl;
    cout << "    Hostel & Mess Facility Management" << endl;
    cout << "    Real-time Eligibility Checking" << endl;
    cout << "    Comprehensive Reporting & Export" << endl;
    cout << string(70, '=') << endl << endl;

    pause_medium();
}

void printMenuHeader(const string& title) {
    cout << "\n" << string(50, '=') << endl;
    cout << " " << title << endl;
    cout << string(50, '=') << endl;
}

void printSuccess(const string& message) {
    cout << "[SUCCESS] " << message << endl;
}

void printError(const string& message) {
    cout << "[ERROR] " << message << endl;
}

void printInfo(const string& message) {
    cout << "[INFO] " << message << endl;
}

void printWarning(const string& message) {
    cout << "[WARNING] " << message << endl;
}

int main() {
    displayWelcomeMessage();

    AdmissionOffice office;
    vector<Student> applicants;
    vector<UnivInfo> created_univs;

    printMenuHeader("SYSTEM SETUP");

    cout << "Use default campus values (recommended)? [Y/n]: ";
    string choice;
    getline(cin, choice);
    if (choice.empty()) choice = "Y";
    bool use_defaults = (choice[0] == 'Y' || choice[0] == 'y');

    int default_boys_rooms = 40;
    int default_girls_rooms = 40;
    int default_mess_capacity = 60;

    if (!use_defaults) {
        cout << "Enter total boys hostel rooms (or press Enter for 40): ";
        string tmp;
        getline(cin, tmp);
        if (!trim(tmp).empty()) default_boys_rooms = stoi(tmp);

        cout << "Enter total girls hostel rooms (or press Enter for 40): ";
        getline(cin, tmp);
        if (!trim(tmp).empty()) default_girls_rooms = stoi(tmp);

        cout << "Enter mess capacity (or press Enter for 60): ";
        getline(cin, tmp);
        if (!trim(tmp).empty()) default_mess_capacity = stoi(tmp);
    } else {
        cout << "Using default values:" << endl;
        cout << "  Boys Hostel: " << default_boys_rooms << " rooms" << endl;
        cout << "  Girls Hostel: " << default_girls_rooms << " rooms" << endl;
        cout << "  Mess Capacity: " << default_mess_capacity << " students" << endl;
    }

    pause_short();

    auto boys_hostel = make_unique<BoysHostel>("Boys Hostel A", default_boys_rooms);
    auto girls_hostel = make_unique<GirlsHostel>("Girls Hostel B", default_girls_rooms);
    office.set_hostels(std::move(boys_hostel), std::move(girls_hostel));

    auto mess = make_unique<Mess>("Main Campus Mess", default_mess_capacity);
    mess->set_plan_price("Basic", 2000);
    mess->set_plan_price("Standard", 3000);
    mess->set_plan_price("Premium", 4500);
    office.set_mess(std::move(mess));

    printSuccess("Campus facilities initialized successfully!");

    auto create_sample_universities = [&]() {
        printInfo("Creating sample universities...");

        {
            auto uni1 = make_unique<University>("TechVille Institute of Technology", 1, 50000);

            unordered_map<string,int> cse_reserved = { {"OBC",5}, {"SC",3}, {"ST",2}, {"EWS",2} };
            uni1->add_branch(make_unique<Branch>("Computer Science", 15, cse_reserved, 1, 5000));

            unordered_map<string,int> ece_reserved = { {"OBC",4}, {"SC",2}, {"EWS",2} };
            uni1->add_branch(make_unique<Branch>("Electronics", 12, ece_reserved, 1, 8000));

            unordered_map<string,int> mech_reserved = { {"OBC",3}, {"SC",2}, {"ST",1} };
            uni1->add_branch(make_unique<Branch>("Mechanical", 10, mech_reserved, 1, 15000));

            UnivInfo u1;
            u1.name = "TechVille Institute of Technology";
            u1.opening_rank = 1;
            u1.closing_rank = 50000;
            u1.branches.push_back({"Computer Science", 15, cse_reserved, 1, 5000});
            u1.branches.push_back({"Electronics", 12, ece_reserved, 1, 8000});
            u1.branches.push_back({"Mechanical", 10, mech_reserved, 1, 15000});
            created_univs.push_back(u1);

            office.add_university(std::move(uni1));
        }

        {
            auto uni2 = make_unique<University>("Global Engineering College", 1, 75000);

            unordered_map<string,int> cse_reserved2 = { {"OBC",4}, {"SC",2}, {"EWS",2} };
            uni2->add_branch(make_unique<Branch>("Computer Science", 12, cse_reserved2, 1, 3000));

            unordered_map<string,int> it_reserved = { {"OBC",3}, {"SC",2}, {"EWS",1} };
            uni2->add_branch(make_unique<Branch>("Information Technology", 10, it_reserved, 1, 7000));

            unordered_map<string,int> civil_reserved = { {"OBC",2}, {"SC",2}, {"ST",1} };
            uni2->add_branch(make_unique<Branch>("Civil Engineering", 8, civil_reserved, 1, 20000));

            UnivInfo u2;
            u2.name = "Global Engineering College";
            u2.opening_rank = 1;
            u2.closing_rank = 75000;
            u2.branches.push_back({"Computer Science", 12, cse_reserved2, 1, 3000});
            u2.branches.push_back({"Information Technology", 10, it_reserved, 1, 7000});
            u2.branches.push_back({"Civil Engineering", 8, civil_reserved, 1, 20000});
            created_univs.push_back(u2);

            office.add_university(std::move(uni2));
        }

        printSuccess("Added two sample universities with branch quotas!");
    };

    bool running = true;
    while (running) {
        printMenuHeader("MAIN MENU");
        cout << "1) Create sample universities (quick setup)" << endl;
        cout << "2) Create custom university (manual input)" << endl;
        cout << "3) Add applicant (manual entry)" << endl;
        cout << "4) Auto-generate demo applicants" << endl;
        cout << "5) List all applicants" << endl;
        cout << "6) Show universities & branch status" << endl;
        cout << "7) Process applications (admissions + allocation)" << endl;
        cout << "8) Show admission summary" << endl;
        cout << "9) Show detailed reports" << endl;
        cout << "10) Export data to CSV" << endl;
        cout << "0) Exit" << endl;
        cout << "Choose an option: ";
        string opt; getline(cin, opt);
        if (opt.empty()) opt = "0";

        if (opt == "1") {
            create_sample_universities();
        }
        else if (opt == "2") {
            printMenuHeader("CREATE CUSTOM UNIVERSITY");
            cout << "Enter name for the new university: ";
            string uname; getline(cin, uname);
            if (trim(uname).empty()) uname = "Custom University";

            cout << "Opening rank (best allowed, default 1): ";
            string o_s; getline(cin, o_s);
            int o = 1;
            try { if(!trim(o_s).empty()) o = stoi(o_s); } catch(...) {}

            cout << "Closing rank (worst allowed, default 100000): ";
            string c_s; getline(cin, c_s);
            int c = 100000;
            try { if(!trim(c_s).empty()) c = stoi(c_s); } catch(...) {}

            auto uni = make_unique<University>(uname, o, c);
            UnivInfo ui;
            ui.name = uname;
            ui.opening_rank = o;
            ui.closing_rank = c;

            cout << "How many branches to add? ";
            string bcount_s;
            getline(cin, bcount_s);
            int bcount = 0;
            try { if(!trim(bcount_s).empty()) bcount = stoi(bcount_s); } catch(...) {}

            for (int i = 0; i < bcount; ++i) {
                cout << "\n--- Branch " << (i+1) << " ---" << endl;
                cout << "Branch name: ";
                string bname; getline(cin, bname);
                if (trim(bname).empty()) bname = "Branch" + to_string(i+1);

                cout << "General seats for " << bname << " (default 10): ";
                string seats_s; getline(cin, seats_s);
                int seats = 10;
                try { if(!trim(seats_s).empty()) seats = stoi(seats_s); } catch(...) {}

                cout << "Branch opening rank (default 1): ";
                string bo_s; getline(cin, bo_s);
                int bo = 1;
                try { if(!trim(bo_s).empty()) bo = stoi(bo_s); } catch(...) {}

                cout << "Branch closing rank (default 100000): ";
                string bc_s; getline(cin, bc_s);
                int bc = 100000;
                try { if(!trim(bc_s).empty()) bc = stoi(bc_s); } catch(...) {}

                unordered_map<string,int> reserved_map;
                cout << "Enter reserved seats for categories (press Enter for 0):" << endl;
                for (auto &cat : CATEGORIES) {
                    if (cat == "General") continue;
                    cout << "  " << cat << " seats: ";
                    string r_s; getline(cin, r_s);
                    int r = 0;
                    try { if(!trim(r_s).empty()) r = stoi(r_s); } catch(...) {}
                    if (r > 0) reserved_map[cat] = r;
                }

                uni->add_branch(make_unique<Branch>(bname, seats, reserved_map, bo, bc));
                ui.branches.push_back({bname, seats, reserved_map, bo, bc});
            }
            office.add_university(std::move(uni));
            created_univs.push_back(ui);
            printSuccess("Custom university '" + uname + "' added successfully!");
        }
        else if (opt == "3") {
            printMenuHeader("ADD APPLICANT");
            string name;
            cout << "Name: ";
            getline(cin, name);
            if (trim(name).empty()) name = "Student" + to_string(applicants.size()+1);

            string income_s;
            cout << "Annual family income (integer): ";
            getline(cin, income_s);
            int income = 500000;
            try { if(!trim(income_s).empty()) income = stoi(income_s); } catch(...) {}

            string rank_s;
            cout << "JEE Rank (integer): ";
            getline(cin, rank_s);
            int rank = 5000;
            try { if(!trim(rank_s).empty()) rank = stoi(rank_s); } catch(...) {}

            string prefs;
            cout << "Preferred branches (comma separated, e.g., Computer Science,Electronics): ";
            getline(cin, prefs);
            vector<string> preferences = split_preferences(prefs);
            if (preferences.empty()) preferences.push_back("Computer Science");

            string contact;
            cout << "Contact number: ";
            getline(cin, contact);
            if (trim(contact).empty()) contact = "0000000000";

            string perc_s;
            cout << "12th percentage (integer): ";
            getline(cin, perc_s);
            int perc = 75;
            try { if(!trim(perc_s).empty()) perc = stoi(perc_s); } catch(...) {}

            string age_s;
            cout << "Age: ";
            getline(cin, age_s);
            int age = 18;
            try { if(!trim(age_s).empty()) age = stoi(age_s); } catch(...) {}

            string gender;
            cout << "Gender (Male/Female): ";
            getline(cin, gender);
            if (trim(gender).empty()) gender = "Male";

            string category;
            cout << "Category (General/OBC/SC/ST/EWS): ";
            getline(cin, category);
            if (trim(category).empty()) category = "General";

            applicants.emplace_back(name, income, rank, preferences, contact, perc, age, gender, category);
            printSuccess("Applicant '" + name + "' added successfully!");
            cout << "Current total applicants: " << applicants.size() << "\n";
        }
        else if (opt == "4") {
            printInfo("Generating demo applicants...");
            applicants.emplace_back("Anita Sharma", 200000, 1200,
                                  vector<string>{"Computer Science","Electronics"},
                                  "9991110001", 92, 18, "Female", "General");
            applicants.emplace_back("Ravi Kumar", 80000, 4200,
                                  vector<string>{"Electronics","Computer Science"},
                                  "9991110002", 88, 18, "Male", "OBC");
            applicants.emplace_back("Priya Singh", 50000, 2500,
                                  vector<string>{"Computer Science","Information Technology"},
                                  "9991110003", 94, 18, "Female", "SC");
            applicants.emplace_back("Aman Verma", 150000, 6000,
                                  vector<string>{"Mechanical","Civil Engineering"},
                                  "9991110004", 82, 19, "Male", "General");
            applicants.emplace_back("Sonal Gupta", 70000, 700,
                                  vector<string>{"Computer Science","Electronics"},
                                  "9991110005", 96, 17, "Female", "General");
            applicants.emplace_back("Karan Mehta", 30000, 1500,
                                  vector<string>{"Information Technology","Computer Science"},
                                  "9991110006", 90, 18, "Male", "EWS");
            printSuccess("Auto-generated 6 demo applicants!");
            cout << "Total applicants: " << applicants.size() << "\n";
        }
        else if (opt == "5") {
            printMenuHeader("APPLICANTS LIST (" + to_string(applicants.size()) + " STUDENTS)");
            if (applicants.empty()) {
                printWarning("No applicants added yet.");
            } else {
                cout << "+-------+----------------------+--------+--------+------+--------+----------+--------------+--------+----------+" << endl;
                cout << "| Roll  | Name                 | JEE    | 12th%  | Age  | Gender | Category | Branch       | Hostel | Mess     |" << endl;
                cout << "+-------+----------------------+--------+--------+------+--------+----------+--------------+--------+----------+" << endl;

                for (const auto &s : applicants) {
                    s.print_details();
                }
                cout << "+-------+----------------------+--------+--------+------+--------+----------+--------------+--------+----------+" << endl;
            }
        }
        else if (opt == "6") {
            printMenuHeader("UNIVERSITIES & BRANCHES STATUS");
            if (created_univs.empty()) {
                printWarning("No universities created yet.");
            } else {
                for (const auto &u : created_univs) {
                    cout << "University: " << u.name
                         << " | Rank Range: " << u.opening_rank << " - " << u.closing_rank << '\n';
                    for (const auto &b : u.branches) {
                        cout << "   - Branch: " << b.name
                             << " | General: " << b.general
                             << " | Rank: " << b.opening_rank << "-" << b.closing_rank << '\n';
                        cout << "     Reserved: ";
                        print_reserved_map(b.reserved);
                        cout << endl;
                    }
                    cout << '\n';
                }
            }

            cout << "Live Branch Seat Status (after admissions):\n";
            auto &unis = office.get_universities();
            if (unis.empty()) {
                printWarning("No universities in system yet.");
            } else {
                for (const auto &up : unis) {
                    if (!up) continue;
                    cout << "University: " << up->get_name() << ":\n";
                    for (auto &kv : up->get_branches()) {
                        const Branch* br = kv.second.get();
                        cout << "   Branch: " << br->get_name()
                             << " | General remaining: " << br->general_remaining()
                             << " | Reserved: ";
                        print_reserved_map(br->get_reserved_map());
                        cout << endl;
                    }
                }
            }
        }
        else if (opt == "7") {
            if (applicants.empty()) {
                printError("No applicants to process. Add some applicants first.");
            } else {
                printMenuHeader("PROCESSING APPLICATIONS");
                cout << "Processing " << applicants.size() << " applications..." << endl;
                cout << "   • Checking eligibility" << endl;
                cout << "   • Allocating branches based on rank & preferences" << endl;
                cout << "   • Assigning hostel accommodation" << endl;
                cout << "   • Allocating mess facilities" << endl;

                pause_medium();
                office.process_applications(applicants);

                printSuccess("Processing complete!");

                office.show_eligibility_report(applicants);
            }
        }
        else if (opt == "8") {
            printMenuHeader("ADMISSION SUMMARY");
            office.show_all_admissions();
        }
        else if (opt == "9") {
            printMenuHeader("DETAILED REPORTS");
            office.show_detailed_report();

            if (!applicants.empty()) {
                cout << "\nShow individual student details? [Y/n]: ";
                string detail_choice;
                getline(cin, detail_choice);
                if (detail_choice.empty() || detail_choice[0] == 'Y' || detail_choice[0] == 'y') {
                    for (const auto& student : applicants) {
                        student.print_detailed_info();
                        cout << endl;
                    }
                }
            }
        }
        else if (opt == "10") {
            printMenuHeader("EXPORT DATA");
            cout << "Enter filename to export (e.g., admissions.csv): ";
            string fn; getline(cin, fn);
            if (trim(fn).empty()) fn = "admissions.csv";

            ofstream out(fn);
            if (!out) {
                printError("Failed to create file: " + fn);
            } else {
                out << "Roll,Name,JEE_Rank,12th_Percentage,Age,Gender,Category,Admitted,Branch,Hostel,Mess_Plan\n";
                for (const auto &s : applicants) {
                    out << s.get_roll() << ','
                        << '"' << s.get_name() << '"' << ','
                        << s.jee_rank << ','
                        << s.get_percentage_12th() << ','
                        << s.get_age() << ','
                        << s.get_gender() << ','
                        << s.get_category() << ','
                        << (s.is_admitted() ? "Yes" : "No") << ','
                        << (s.is_admitted() ? s.get_preferences()[0] : "NA") << ','
                        << (s.has_hostel() ? "Yes" : "No") << ','
                        << (s.has_mess() ? s.get_mess_plan() : "No") << '\n';
                }
                out.close();
                printSuccess("Data exported successfully to: " + fn);
                cout << "Records exported: " << applicants.size() << endl;
            }
        }
        else if (opt == "0") {
            running = false;
            printMenuHeader("THANK YOU");
            cout << "Thank you for using the University Admission System!" << endl;
            cout << "Goodbye!" << endl;
        }
        else {
            printError("Invalid option. Please try again.");
        }

        if (running) {
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }
    }
    return 0;
}