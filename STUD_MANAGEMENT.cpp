#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>
using namespace std;

class Student {
private:
    int id;
    string name, course, phone;
    double marks[3];

public:
    Student() : id(0), name(""), course(""), phone("") {
        marks[0] = marks[1] = marks[2] = 0.0;
    }

    Student(int studentId, const string& studentName,
            const string& studentCourse, const string& studentPhone,
            double m1, double m2, double m3)
        : id(studentId), name(studentName), course(studentCourse),
          phone(studentPhone) {
        marks[0] = m1; marks[1] = m2; marks[2] = m3;
    }

    int getId() const { return id; }
    string getName() const { return name; }
    string getCourse() const { return course; }

    double percentage() const {
        return (marks[0] + marks[1] + marks[2]) / 3.0;
    }

    string grade() const {
        double p = percentage();
        if (p >= 90) return "A+";
        if (p >= 80) return "A";
        if (p >= 70) return "B";
        if (p >= 60) return "C";
        if (p >= 50) return "D";
        return "F";
    }

    void display() const {
        cout << left << setw(8) << id << setw(22) << name
             << setw(14) << course << setw(12) << phone
             << fixed << setprecision(1) << setw(10) << percentage()
             << grade() << '\n';
    }

    void editDetails() {
        cout << "Enter new name: ";
        getline(cin, name);
        cout << "Enter new course: ";
        getline(cin, course);
        cout << "Enter new phone: ";
        getline(cin, phone);
    }

    void editMarks() {
        for (int i = 0; i < 3; ++i) {
            while (true) {
                cout << "Enter marks for subject " << i + 1 << " (0-100): ";
                if (cin >> marks[i] && marks[i] >= 0 && marks[i] <= 100) break;
                cout << "Invalid marks. Please enter a value from 0 to 100.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string serialize() const {
        // Text-file format: id|name|course|phone|mark1|mark2|mark3
        return to_string(id) + "|" + name + "|" + course + "|" + phone + "|" +
               to_string(marks[0]) + "|" + to_string(marks[1]) + "|" +
               to_string(marks[2]);
    }

    static bool deserialize(const string& line, Student& s) {
        vector<string> fields;
        string part;
        size_t start = 0, pos;
        while ((pos = line.find('|', start)) != string::npos) {
            fields.push_back(line.substr(start, pos - start));
            start = pos + 1;
        }
        fields.push_back(line.substr(start));
        if (fields.size() != 7) return false;
        try {
            s = Student(stoi(fields[0]), fields[1], fields[2], fields[3],
                        stod(fields[4]), stod(fields[5]), stod(fields[6]));
        } catch (...) {
            return false;
        }
        return true;
    }

    friend class StudentManager;
};

class StudentManager {
private:
    vector<Student> students;
    const string filename = "students.txt";

    int findIndex(int id) const {
        for (size_t i = 0; i < students.size(); ++i)
            if (students[i].id == id) return static_cast<int>(i);
        return -1;
    }

    void saveToFile() const {
        ofstream file(filename);
        if (!file) {
            cout << "Error: unable to save records.\n";
            return;
        }
        for (const Student& s : students) file << s.serialize() << '\n';
    }

    void loadFromFile() {
        ifstream file(filename);
        if (!file) return; // First run: file is created when a record is saved.
        string line;
        while (getline(file, line)) {
            Student s;
            if (Student::deserialize(line, s)) students.push_back(s);
        }
    }

    static int readId() {
        int id;
        while (true) {
            cout << "Enter student ID: ";
            if (cin >> id && id > 0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return id;
            }
            cout << "Please enter a positive numeric ID.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

public:
    StudentManager() { loadFromFile(); }

    void addStudent() {
        int id = readId();
        if (findIndex(id) != -1) {
            cout << "A student with this ID already exists.\n";
            return;
        }

        string name, course, phone;
        cout << "Enter student name: "; getline(cin, name);
        cout << "Enter course: "; getline(cin, course);
        cout << "Enter phone number: "; getline(cin, phone);

        Student s(id, name, course, phone, 0, 0, 0);
        cout << "Add marks now? (y/n): ";
        char choice; cin >> choice;
        if (choice == 'y' || choice == 'Y') s.editMarks();
        else cin.ignore(numeric_limits<streamsize>::max(), '\n');

        students.push_back(s);
        saveToFile();
        cout << "Student added successfully.\n";
    }

    void displayAll() const {
        if (students.empty()) {
            cout << "No student records found.\n";
            return;
        }
        cout << "\n" << left << setw(8) << "ID" << setw(22) << "NAME"
             << setw(14) << "COURSE" << setw(12) << "PHONE"
             << setw(10) << "AVERAGE" << "GRADE\n";
        cout << string(76, '-') << '\n';
        for (const Student& s : students) s.display();
    }

    void searchStudent() const {
        int id = readId();
        int index = findIndex(id);
        if (index == -1) cout << "Student not found.\n";
        else {
            cout << "\nStudent record:\n";
            cout << "ID: " << students[index].id << '\n'
                 << "Name: " << students[index].name << '\n'
                 << "Course: " << students[index].course << '\n'
                 << "Phone: " << students[index].phone << '\n'
                 << "Average: " << fixed << setprecision(2)
                 << students[index].percentage() << "%\n"
                 << "Grade: " << students[index].grade() << '\n';
        }
    }

    void updateStudent() {
        int id = readId();
        int index = findIndex(id);
        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }
        cout << "1. Update personal details\n2. Update marks\nEnter choice: ";
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 1) students[index].editDetails();
        else if (choice == 2) students[index].editMarks();
        else {
            cout << "Invalid choice.\n";
            return;
        }
        saveToFile();
        cout << "Student record updated successfully.\n";
    }

    void deleteStudent() {
        int id = readId();
        int index = findIndex(id);
        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }
        students.erase(students.begin() + index);
        saveToFile();
        cout << "Student record deleted successfully.\n";
    }

    void showResult() const {
        int id = readId();
        int index = findIndex(id);
        if (index == -1) {
            cout << "Student not found.\n";
            return;
        }
        const Student& s = students[index];
        cout << "\nRESULT\n";
        cout << "Student: " << s.name << " (ID: " << s.id << ")\n";
        cout << "Average: " << fixed << setprecision(2)
             << s.percentage() << "%\n";
        cout << "Grade: " << s.grade() << '\n';
        if (s.percentage() >= 40) cout << "Status: Pass\n";
        else cout << "Status: Needs improvement\n";
    }

    void run() {
        int choice;
        do {
            cout << "\n========================================\n"
                 << "       STUDENT MANAGEMENT SYSTEM\n"
                 << "========================================\n"
                 << "1. Add New Student\n"
                 << "2. Display All Students\n"
                 << "3. Search Student\n"
                 << "4. Update Student\n"
                 << "5. Delete Student\n"
                 << "6. Calculate Result\n"
                 << "7. Exit\n"
                 << "----------------------------------------\n"
                 << "Enter your choice: ";

            if (!(cin >> choice)) {
                cout << "Invalid input. Enter a number from 1 to 7.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
                case 1: addStudent(); break;
                case 2: displayAll(); break;
                case 3: searchStudent(); break;
                case 4: updateStudent(); break;
                case 5: deleteStudent(); break;
                case 6: showResult(); break;
                case 7: cout << "Thank you for using the system.\n"; break;
                default: cout << "Invalid choice. Try again.\n";
            }
        } while (choice != 7);
    }
};

int main() {
    StudentManager manager;
    manager.run();
    return 0;
}
