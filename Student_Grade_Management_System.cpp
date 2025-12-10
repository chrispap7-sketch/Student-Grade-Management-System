#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>  

using namespace std;

// Δομή για την αποθήκευση των δεδομένων μαθητή
struct Student {
    string id;
    vector<string> data; // Όλα τα δεδομένα της γραμμής
    vector<double> grades; // Βαθμοί για κάθε μάθημα
    
    // Υπολογισμός μέσου όρου
    double average() {
        if (grades.empty()) return 0.0;
        double sum = 0;
        int count = 0;
        for (double grade : grades) {
            if (grade > 0) { // Μετράμε μόνο θετικούς βαθμούς
                sum += grade;
                count++;
            }
        }
        if (count == 0) return 0.0;
        return sum / count;
    }
    
    // Εμφάνιση στοιχείων μαθητή
    void print(const vector<string>& headers) {
        cout << "---------------------------------------------------\n";  
        cout << "ID: " << id << '\n';
        cout << "Name: " << data[1] << " " << data[2] << '\n'; // Όνομα και επώνυμο στις στήλες 2 και 3
        cout << fixed << setprecision(1);
        cout << "Average: " << average() << '\n';
        
        // Εμφάνιση βαθμών αν υπάρχουν
        if (!grades.empty()) {
            cout << "Grades:\n";
            for (size_t i = 0; i < grades.size(); i++) {
                if (grades[i] > 0) { // Εμφανίζουμε μόνο θετικούς βαθμούς
                    if (i + 3 < headers.size()) { // Βλέπουμε αν υπάρχει όνομα για αυτή τη στήλη
                        cout << "  " << headers[i + 3] << ": " << grades[i] << '\n';
                    } else {
                        cout << "  Subject " << (i + 1) << ": " << grades[i] << '\n';
                    }
                }
            }
        }
        cout << "---------------------------------------------------\n"; 
    }
};

// Συνάρτηση διαχωρισμού γραμμής CSV
vector<string> splitCSV(const string& line) {
    vector<string> result;
    stringstream ss(line);
    string item;
    
    while (getline(ss, item, ',')) {
        // Αφαίρεση κενών στην αρχή και στο τέλος
        size_t start = item.find_first_not_of(" \t");
        size_t end = item.find_last_not_of(" \t");
        if (start != string::npos && end != string::npos)
            result.push_back(item.substr(start, end - start + 1));
        else
            result.push_back("");
    }
    
    return result;
}

// Συνάρτηση εγγραφής δεδομένων σε CSV
string joinCSV(const vector<string>& data) {
    string result;
    for (size_t i = 0; i < data.size(); ++i) {
        // Εάν το περιεχόμενο έχει κόμμα, το βάζουμε σε εισαγωγικά
        if (data[i].find(',') != string::npos || data[i].find('"') != string::npos) {
            string escaped = data[i];
            replace(escaped.begin(), escaped.end(), '"', '\"');
            result += "\"" + escaped + "\"";
        } else {
            result += data[i];
        }
        if (i < data.size() - 1)
            result += ",";
    }
    return result;
}

int main() {
    string filename = "student.csv", studentId;
    vector<Student> students;
    vector<string> headers;
    string line;
    bool foundStudent = false;
    int studentIndex = -1;
    
    //Προσθήκη μαθητή
    string newStudent;
    cout << "Θέλετε να καταχωρήσετε νέο σπουδαστή;\n";
    cout << "Αν ναι, πατήστε y: ";
    cin >> newStudent;
    cin.ignore(); // Αδειάζει το buffer για να μην παρεμβαίνει στο επόμενο getline

    if (newStudent == "y"){
        ofstream out(filename, ios::app); //ΑΝΟΙΓΕΙ ΤΟ ΑΡΧΕΙΟ ΚΑΙ ΔΕΝ ΚΑΝΕΙ OVERWRITE

        if (!out){
            cerr << "Σφάλμα! Δεν ανοίγει το αρχείο!" << filename;
            return 1;
        }

        string name, surname;
        cout << "Εισαγωγή ID. Αφήστε κενό για τερματισμό! \n";
        
        while(true){
            cout << "ID: ";
            getline(cin, studentId); // Χρησιμοποίησε getline για να διαβάσεις κενό
            if (studentId.empty()){ // Έλεγχος για άδειο string
                break;
            }
        
            cout << "\nΌνομα: ";
            getline(cin, name);

            cout << "\nΕπώνυμο: ";
            getline(cin, surname);

            out << studentId << " , " << name << " , " << surname << '\n'; //ΓΡΑΦΟΥΜΕ ΤΗΝ ΓΡΑΜΜΗ ΚΑΙ NEW LINE
        }
        out.close();
        cout << "Η νέα εγγραφή ολοκληρώθηκε στο " << filename << '\n';
    }
    
    // Εισαγωγή ID μαθητή για αναζήτηση
    cout << "\nΔώσε το ID του μαθητή: ";
    getline(cin, studentId);
    
    // Άνοιγμα αρχείου
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Σφάλμα: Δεν μπορώ να ανοίξω το αρχείο " << filename << endl;
        return 1;
    }

    // Διάβασμα κεφαλίδας (πρώτη γραμμή)
    if (getline(inputFile, line)) {
        headers = splitCSV(line);
    }
    
    // Διάβασμα υπόλοιπων γραμμών
    int lineNum = 0;
    while (getline(inputFile, line)) {
        vector<string> rowData = splitCSV(line);
        
        if (!rowData.empty()) {
            Student student;
            student.id = rowData[0]; // ID είναι στην 1η στήλη
            student.data = rowData;
            
            // Μετατροπή βαθμών από string σε double
            // Οι βαθμοί αρχίζουν από τη 4η στήλη (index 3)
            for (size_t i = 3; i < rowData.size(); i++) {
                if (!rowData[i].empty()) {
                    try {
                        double grade = stod(rowData[i]);
                        student.grades.push_back(grade);
                    } catch (const invalid_argument&) {
                        // Αν δεν είναι αριθμός, βάζουμε 0
                        student.grades.push_back(0.0);
                    }
                } else {
                    student.grades.push_back(0.0);
                }
            }
            
            // Αν το rowData έχει λιγότερες στήλες από τις headers, συμπληρώνουμε με κενά
            while (student.data.size() < headers.size()) {
                student.data.push_back("");
                if (student.data.size() >= 3) { // Μόνο για στήλες βαθμών
                    student.grades.push_back(0.0);
                }
            }
            
            // Έλεγχος αν αυτός είναι ο μαθητής που ψάχνουμε
            if (student.id == studentId) {
                foundStudent = true;
                studentIndex = lineNum;
            }
            
            students.push_back(student);
            lineNum++;
        }
    }
    inputFile.close();
    
    if (!foundStudent) {
        cout << "Ο μαθητής με ID " << studentId << " δεν βρέθηκε." << endl;
        return 1;
    }
    
    // Εμφάνιση των διαθέσιμων μαθημάτων
    cout << "\nΤα διαθέσιμα μαθήματα είναι:" << endl;
    cout << "==================================" << endl;
    
    for (size_t i = 3; i < headers.size(); i++) {
        cout << i-2 << ". " << headers[i];
        // Εμφάνιση τρέχοντος βαθμού αν υπάρχει
        if (studentIndex >= 0 && studentIndex < students.size()) {
            if (i - 3 < students[studentIndex].grades.size()) {
                double currentGrade = students[studentIndex].grades[i-3];
                if (currentGrade > 0) {
                    cout << " (Τρέχων βαθμός: " << currentGrade << ")";
                }
            }
        }
        cout << endl;
    }

    // Loop για πολλαπλή εισαγωγή βαθμών
    char addAnother = 'y';
    
    while (addAnother == 'y' || addAnother == 'Y') {
        cout << "\nΕπέλεξε μάθημα (Αριθμός): ";
        int choice;
        cin >> choice;
        cin.ignore(); // Αδειάζει το buffer

        // Έλεγχος αν έβαλε σωστή επιλογή
        if (choice < 1 || choice > headers.size() - 3) {
            cout << "Λάθος επιλογή!" << endl;
            return 1;
        }

        // Η πραγματική στήλη στο αρχείο (προσθέτουμε 3 γιατί ξεκινάμε από τη 4η)
        int columnIndex = choice + 2;
        cout << "Επέλεξες το μάθημα: " << headers[columnIndex] << endl;

        // Ζητάμε τον βαθμό από τον χρήστη
        cout << "Δώσε βαθμό για το μάθημα " << headers[columnIndex] << " (0-10): ";
        double grade;
        cin >> grade;
        cin.ignore(); // Αδειάζει το buffer
        
        // Έλεγχος ότι ο βαθμός είναι μεταξύ 0 και 10
        if (grade < 0.0 || grade > 10.0) {
            cout << "Λάθος βαθμός! Ο βαθμός πρέπει να είναι από 0 έως 10." << endl;
            return 1;
        }

        // Μορφοποίηση με 1 δεκαδικό ψηφίο
        stringstream gradeStream;
        gradeStream << fixed << setprecision(1) << grade;
        string gradeStr = gradeStream.str();
        
        // Ενημερώνουμε τον βαθμό στον μαθητή
        Student& student = students[studentIndex];
        
        // Αν τα data δεν έχουν αρκετές στήλες, προσθέτουμε κενά μέχρι να φτάσουμε στη στήλη που θέλουμε
        while (student.data.size() <= columnIndex) {
            student.data.push_back("");
        }
        
        // Αν τα grades δεν έχουν αρκετές θέσεις, συμπληρώνουμε
        while (student.grades.size() <= columnIndex - 3) {
            student.grades.push_back(0.0);
        }
        
        // Αλλάζουμε τον βαθμό
        student.data[columnIndex] = gradeStr;
        student.grades[columnIndex - 3] = grade; // Αφαιρούμε 3 γιατί οι βαθμοί ξεκινάνε από index 0
        
        // Ενημερώνουμε τον πίνακα students
        students[studentIndex] = student;
        
        cout << "\nΟ βαθμός " << grade << " προστέθηκε επιτυχώς στο μάθημα " 
             << headers[columnIndex] << " για τον μαθητή με ID " << studentId << "!" << endl;
        
        // Ρωτάμε αν θέλει να βάλει βαθμό σε άλλο μάθημα
        cout << "\nΘέλετε να βάλετε βαθμό σε άλλο μάθημα; (y/n): ";
        cin >> addAnother;
        cin.ignore(); // Αδειάζει το buffer
        
        // Εμφάνιση των μαθημάτων ξανά με τους ενημερωμένους βαθμούς
        if (addAnother == 'y' || addAnother == 'Y') {
            cout << "\nΤα διαθέσιμα μαθήματα είναι:" << endl;
            cout << "==================================" << endl;
            
            for (size_t i = 3; i < headers.size(); i++) {
                cout << i-2 << ". " << headers[i];
                // Εμφάνιση τρέχοντος βαθμού αν υπάρχει
                if (studentIndex >= 0 && studentIndex < students.size()) {
                    if (i - 3 < students[studentIndex].grades.size()) {
                        double currentGrade = students[studentIndex].grades[i-3];
                        if (currentGrade > 0) {
                            cout << " (Τρέχων βαθμός: " << currentGrade << ")";
                        }
                    }
                }
                cout << endl;
            }
        }
    }
    
    // Γράφουμε όλο το αρχείο ξανά με τις αλλαγές
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Σφάλμα: Δεν μπορώ να ανοίξω το αρχείο για εγγραφή." << endl;
        return 1;
    }
    
    // Γράφουμε την κεφαλίδα
    outputFile << joinCSV(headers) << endl;
    
    // Γράφουμε όλους τους μαθητές
    for (const auto& s : students) {
        outputFile << joinCSV(s.data) << endl;
    }
    
    outputFile.close();
    
    cout << "\nΕνημερωμένα στοιχεία μαθητή:" << endl;
    students[studentIndex].print(headers);
    
    return 0;
}