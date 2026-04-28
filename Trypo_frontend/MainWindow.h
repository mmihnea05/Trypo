#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QScrollArea>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QCheckBox>

struct Facility {
    QString name;
};

struct Room {
    QString type;    // single room, double room, triple room
    int beds;
    QList<Facility> facilities;
    double basePrice;
    bool hasSofa;    // creste capacitatea cu 1
};

struct Accommodation {
    QString name;
    QString location;
    QString address;
    double discountPercent;
    QString promoName;
    QList<Room> rooms;
};

// aici salvam istoricul unei cazari
struct BookingHistory {
    QString hotelName;
    QString dateRange;
    QString status;
};

// datele despre utilizator
struct User {
    QString name, email, password, phone, dob, country, gender, address;
};

// slot = mecanism prin care butoanele apeleaza functii
// click Login -> call processLogin
class MainWindow : public QMainWindow {
    Q_OBJECT // macro obligatoriu cand lucram cu sloturi

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void goToRegister();
    void goToLogin();
    void processLogin();
    void processRegister();
    void goToUserProfile();
    void backToMainApp();
    void filterAccommodations(const QString &query);
    void filterRooms(const QString &query);
    void openAccommodationDetails(const Accommodation &acc);

private:
    // initializam interfata cu utilizatorul (cele 5 interfete)
    void setupUi();
    QWidget* createLoginWidget();
    QWidget* createRegisterWidget();
    QWidget* createMainAppWidget();
    QWidget* createUserProfileWidget();
    QWidget* createDetailsWidget();

    void populateAccommodations(const QString &filter = "");
    void displayRooms(const QString &filter = "");
    void clearRegisterFields();

    // obiecte in interfata de register
    QStackedWidget *stackedWidget;
    QLineEdit *loginEmailInput, *loginPasswordInput;
    QLineEdit *regNameInput, *regEmailInput, *regPasswordInput, *regPhoneInput, *regAddressInput;
    QDateEdit *regDobInput;
    QComboBox *regCountryInput, *regGenderInput;

    // obiecte in interfata principala
    QLineEdit *searchBarInput;
    QWidget *accommodationsContainer;
    QVBoxLayout *accommodationsLayout;

    // interfata my profile
    QLabel *lblNameVal, *lblEmailVal, *lblPhoneVal, *lblDobVal, *lblCountryVal, *lblGenderVal, *lblAddressVal;

    // ob. in interfata unei unitati de cazare
    QLabel *detName, *detAddress, *detPromo;
    QLineEdit *roomSearchBar;
    QVBoxLayout *roomsLayout;
    QCheckBox *cbBalcony, *cbFridge, *cbAC, *cbTV, *cbWifi, *cbSofa;
    Accommodation currentAccommodationInDetails;

    // dictionar key, value unde se cauta user-ul dupa adresa de mail
    QMap<QString, User> userDatabase;
    // utilizatorul sesiunii curente
    User currentUser;
    // lista unitatilor de cazare din aplicatie
    QList<Accommodation> allAccommodations;
    // istoricul rezervarilor
    QList<BookingHistory> userBookings;
};

#endif