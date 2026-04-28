#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QFrame>
#include <QRegularExpression>

// stiluri
const QString primaryBtnStyle = "QPushButton { background-color: #2563eb; color: white; border: none; border-radius: 6px; padding: 10px 20px; font-weight: bold; font-size: 14px; } QPushButton:hover { background-color: #60a5fa; }";
const QString secondaryBtnStyle = "QPushButton { background-color: transparent; color: #2563eb; border: 2px solid #2563eb; border-radius: 6px; padding: 8px 16px; font-weight: bold; font-size: 13px; } QPushButton:hover { background-color: #f8fafc; color: #1d4ed8; }";
const QString dangerBtnStyle = "QPushButton { background-color: #ef4444; color: white; border: none; border-radius: 6px; padding: 10px 20px; font-weight: bold; font-size: 14px; } QPushButton:hover { background-color: #fca5a5; }";
const QString lineEditStyle = "QLineEdit { padding: 10px; border: 1px solid #475569; border-radius: 5px; background-color: #1e293b; color: #f8fafc; font-size: 14px; } QLineEdit:focus { border: 2px solid #3b82f6; }";
const QString dropDownStyle = "QComboBox, QDateEdit { padding: 5px 10px; border: 1px solid #475569; border-radius: 5px; background-color: #1e293b; color: #f8fafc; font-size: 14px; }";
const QString globalLabelStyle = "QLabel { border: none; background: transparent; outline: none; padding: 0px; margin: 0px; }";
const QString checkBoxStyle = "QCheckBox { color: white; font-size: 14px; } QCheckBox::indicator { width: 18px; height: 18px; border-radius: 4px; border: 2px solid #475569; background-color: #1e293b; } QCheckBox::indicator:checked { background-color: #2563eb; border: 2px solid #2563eb; }";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Trypo");
    resize(1000, 800);
    this->setStyleSheet("QMainWindow { background-color: #0f172a; } " + globalLabelStyle);

    cbBalcony = cbFridge = cbAC = cbTV = cbWifi = cbSofa = nullptr;

    // date pentru testare
    allAccommodations = {
        {"Grand Hotel Trypo", "Bucharest, Romania", "Calea Victoriei 12", 0.15, "Early Discount",
         {{"Single Room", 1, {{"WiFi"}, {"TV"}}, 100.0, false},
          {"Double Room", 2, {{"AC"}, {"Fridge"}, {"Balcony"}}, 150.0, false}}},

        {"Retezat Cabin", "Retezat Mountains, Romania", "Parcul National Retezat", 0.10, "Off-season Discount",
         {{"Triple Room", 3, {{"Balcony"}, {"Sofa"}, {"Fridge"}}, 200.0, true}}},

        {"Medieval Plaza", "Sibiu, Romania", "Piata Mare 5", 0.05, "City Break",
         {{"Double Room", 2, {{"WiFi"}, {"AC"}}, 140.0, false},
          {"Triple Room", 3, {{"TV"}, {"Sofa"}}, 190.0, true}}},

        {"Sinaia Guesthouse", "Sinaia, Romania", "Strada Peles 1", 0.0, "None",
         {{"Single Room", 1, {{"WiFi"}, {"Balcony"}}, 90.0, false},
          {"Double Room", 2, {{"Fridge"}, {"Sofa"}}, 160.0, true}}},

        {"Cluj Central Stay", "Cluj-Napoca, Romania", "Strada Eroilor 10", 0.0, "None",
         {{"Double Room", 2, {{"WiFi"}, {"AC"}, {"TV"}}, 130.0, false}}},

        {"Danube Lodge", "Tulcea, Romania", "Delta Dunarii", 0.15, "Wildlife Explorer",
         {{"Triple Room", 3, {{"Balcony"}, {"Sofa"}}, 180.0, true}}},

        {"Iasi Business Hotel", "Iasi, Romania", "Strada Palat 20", 0.10, "Business Promo",
         {{"Single Room", 1, {{"WiFi"}, {"AC"}}, 120.0, false},
          {"Double Room", 2, {{"WiFi"}, {"Fridge"}}, 170.0, false}}},

        {"Vatra Dornei Spa", "Vatra Dornei, Romania", "Strada Sportului 5", 0.20, "Winter Relaxation",
         {{"Double Room", 2, {{"AC"}, {"Balcony"}}, 145.0, false},
          {"Triple Room", 3, {{"Fridge"}, {"Sofa"}}, 210.0, true}}}
    };

    userBookings = {
        {"Retezat Cabin", "Jan 12 - Jan 15, 2026", "Finished"},
        {"Grand Hotel Trypo", "April 10 - April 12, 2026", "Finished"}
    };

    User testUser;
    testUser.name = "Mihnea"; testUser.email = "mm@gmail.com"; testUser.password = "mihnea";
    testUser.phone = "0722111222"; testUser.dob = "10 May 1995"; testUser.country = "Romania";
    testUser.gender = "Male"; testUser.address = "Strada Primaverii nr. 1, Bucuresti";
    userDatabase.insert(testUser.email, testUser);

    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(createLoginWidget());      // 0
    stackedWidget->addWidget(createRegisterWidget());   // 1
    stackedWidget->addWidget(createMainAppWidget());    // 2
    stackedWidget->addWidget(createUserProfileWidget());// 3
    stackedWidget->addWidget(createDetailsWidget());    // 4
    setCentralWidget(stackedWidget);
}

void MainWindow::processRegister() {
    QString em = regEmailInput->text().trimmed();
    QString ps = regPasswordInput->text();
    if (regNameInput->text().isEmpty() || em.isEmpty() || ps.isEmpty()) { QMessageBox::warning(this, "Error", "Fill all fields."); return; }
    if (!em.contains("@")) { QMessageBox::warning(this, "Error", "Invalid email."); return; }
    if (ps.length() < 6 || !QRegularExpression("[A-Z]").match(ps).hasMatch() || !QRegularExpression("[^a-zA-Z0-9]").match(ps).hasMatch()) {
        QMessageBox::warning(this, "Error", "Weak password."); return;
    }
    User newUser = {regNameInput->text().trimmed(), em, ps, regPhoneInput->text().trimmed(), regDobInput->date().toString("dd MMM yyyy"), regCountryInput->currentText(), regGenderInput->currentText(), regAddressInput->text().trimmed()};
    userDatabase.insert(em, newUser);
    QMessageBox::information(this, "Success", "Account created!");
    goToLogin();
}

void MainWindow::processLogin() {
    QString em = loginEmailInput->text().trimmed();
    QString ps = loginPasswordInput->text();
    if (!userDatabase.contains(em) || userDatabase[em].password != ps) { QMessageBox::warning(this, "Login Failed", "Invalid credentials."); return; }
    currentUser = userDatabase[em];
    lblNameVal->setText(currentUser.name); lblEmailVal->setText(currentUser.email);
    lblPhoneVal->setText(currentUser.phone.isEmpty() ? "Not provided" : currentUser.phone);
    lblDobVal->setText(currentUser.dob); lblCountryVal->setText(currentUser.country);
    lblGenderVal->setText(currentUser.gender); lblAddressVal->setText(currentUser.address);
    if (searchBarInput) searchBarInput->clear();
    loginPasswordInput->clear();
    stackedWidget->setCurrentIndex(2);
}

QWidget* MainWindow::createLoginWidget() {
    QWidget *w = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(w);
    l->setAlignment(Qt::AlignCenter);
    QLabel *t = new QLabel("Welcome back to Trypo", w);
    t->setStyleSheet("font-size: 28px; font-weight: bold; color: #f8fafc; margin-bottom: 25px;");
    loginEmailInput = new QLineEdit(w); loginEmailInput->setPlaceholderText("Email"); loginEmailInput->setStyleSheet(lineEditStyle);
    loginPasswordInput = new QLineEdit(w); loginPasswordInput->setPlaceholderText("Password");
    loginPasswordInput->setEchoMode(QLineEdit::Password); loginPasswordInput->setStyleSheet(lineEditStyle);
    QPushButton *lb = new QPushButton("Login", w); lb->setStyleSheet(primaryBtnStyle); lb->setCursor(Qt::PointingHandCursor);
    QPushButton *rb = new QPushButton("Create account", w); rb->setStyleSheet(secondaryBtnStyle); rb->setCursor(Qt::PointingHandCursor);
    l->addWidget(t); l->addWidget(loginEmailInput); l->addWidget(loginPasswordInput);
    l->addSpacing(15); l->addWidget(lb); l->addWidget(rb);
    connect(lb, &QPushButton::clicked, this, &MainWindow::processLogin);
    connect(rb, &QPushButton::clicked, this, &MainWindow::goToRegister);
    return w;
}

QWidget* MainWindow::createRegisterWidget() {
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(50, 20, 50, 20);
    QLabel *t = new QLabel("Register to Trypo", w);
    t->setStyleSheet("font-size: 26px; font-weight: bold; color: #f8fafc; margin-bottom: 20px;");
    QFormLayout *fl = new QFormLayout(); fl->setSpacing(12);
    regNameInput = new QLineEdit(); regNameInput->setStyleSheet(lineEditStyle);
    regEmailInput = new QLineEdit(); regEmailInput->setStyleSheet(lineEditStyle);
    regPasswordInput = new QLineEdit(); regPasswordInput->setEchoMode(QLineEdit::Password); regPasswordInput->setStyleSheet(lineEditStyle);
    regPhoneInput = new QLineEdit(); regPhoneInput->setStyleSheet(lineEditStyle);
    regAddressInput = new QLineEdit(); regAddressInput->setStyleSheet(lineEditStyle);
    regDobInput = new QDateEdit(QDate(2000, 1, 1));
    regDobInput->setCalendarPopup(true); regDobInput->setStyleSheet(dropDownStyle);
    // set echo mode -> ascundem parola cand o scriem de la tastatura

    QCalendarWidget *calendar = regDobInput->calendarWidget();
    calendar->setStyleSheet(
        "QCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #0f172a; padding: 4px; }"
        "QCalendarWidget QToolButton { color: white; background-color: transparent; border: none; font-weight: bold; font-size: 14px; padding: 4px 8px; margin: 0px 2px; }"
        "QCalendarWidget QToolButton:hover { background-color: #2563eb; border-radius: 4px; }"
        "QCalendarWidget QMenu { background-color: #1e293b; color: white; border: 1px solid #475569; }"
        "QCalendarWidget QSpinBox { background-color: #1e293b; color: white; border: 1px solid #475569; border-radius: 3px; padding: 2px 4px; min-width: 65px; margin-left: 10px; }"
        "QCalendarWidget QSpinBox::up-button, QCalendarWidget QSpinBox::down-button { width: 16px; }"
        "QCalendarWidget QAbstractItemView:enabled { background-color: #1e293b; color: white; selection-background-color: #2563eb; selection-color: white; border: none; outline: none; }"
        "QCalendarWidget QAbstractItemView:disabled { color: #475569; }"
        );

    regCountryInput = new QComboBox(); regCountryInput->addItems({"Romania", "Moldova", "UK", "Germany"}); regCountryInput->setStyleSheet(dropDownStyle);
    regGenderInput = new QComboBox(); regGenderInput->addItems({"Male", "Female", "Unspecified"}); regGenderInput->setStyleSheet(dropDownStyle);
    QString ls = "font-weight: bold; color: #94a3b8; font-size: 14px;";
    auto addR = [&](QString txt, QWidget* in) { QLabel *l = new QLabel(txt); l->setStyleSheet(ls); fl->addRow(l, in); };
    addR("Full Name:", regNameInput); addR("Email:", regEmailInput); addR("Password:", regPasswordInput);
    addR("Phone:", regPhoneInput); addR("Date of Birth:", regDobInput); addR("Country:", regCountryInput);
    addR("Gender:", regGenderInput); addR("Address:", regAddressInput);
    ml->addWidget(t); ml->addLayout(fl);
    QPushButton *rb = new QPushButton("Register Now", w); rb->setStyleSheet(primaryBtnStyle);
    QPushButton *bb = new QPushButton("Back to Login", w); bb->setStyleSheet(secondaryBtnStyle);
    ml->addSpacing(20); ml->addWidget(rb); ml->addWidget(bb);
    connect(rb, &QPushButton::clicked, this, &MainWindow::processRegister);
    connect(bb, &QPushButton::clicked, this, &MainWindow::goToLogin);
    return w;
}

QWidget* MainWindow::createMainAppWidget() {
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(20, 20, 20, 20);
    QHBoxLayout *hl = new QHBoxLayout();
    QLabel *t = new QLabel("Recommended for you");
    t->setStyleSheet("font-size: 22px; font-weight: bold; color: #f8fafc;");
    searchBarInput = new QLineEdit(); searchBarInput->setPlaceholderText("Where do you want to go?"); searchBarInput->setStyleSheet(lineEditStyle);
    connect(searchBarInput, &QLineEdit::textChanged, this, &MainWindow::filterAccommodations);
    QPushButton *pb = new QPushButton("👤 My Profile"); pb->setStyleSheet(secondaryBtnStyle); pb->setCursor(Qt::PointingHandCursor);
    QPushButton *lb = new QPushButton("Logout"); lb->setStyleSheet(dangerBtnStyle);
    hl->addWidget(t); hl->addStretch(); hl->addWidget(searchBarInput); hl->addWidget(pb); hl->addWidget(lb);
    ml->addLayout(hl);

    QScrollArea *sa = new QScrollArea(); sa->setWidgetResizable(true); sa->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    accommodationsContainer = new QWidget(); accommodationsLayout = new QVBoxLayout(accommodationsContainer);
    accommodationsLayout->setAlignment(Qt::AlignTop); sa->setWidget(accommodationsContainer);
    ml->addWidget(sa);
    populateAccommodations();
    // legatura intre butoane si sloturi (functii)
    connect(pb, &QPushButton::clicked, this, &MainWindow::goToUserProfile);
    connect(lb, &QPushButton::clicked, this, &MainWindow::goToLogin);
    return w;
}

QWidget* MainWindow::createUserProfileWidget() {
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(40, 30, 40, 30);
    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *bb = new QPushButton("← Back to Explore"); bb->setStyleSheet(secondaryBtnStyle);
    connect(bb, &QPushButton::clicked, this, &MainWindow::backToMainApp);
    QLabel *pt = new QLabel("Account Settings"); pt->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    h->addWidget(bb); h->addSpacing(20); h->addWidget(pt); h->addStretch();
    ml->addLayout(h);
    ml->addSpacing(30);
    QLabel *sub1 = new QLabel("Customer Information"); sub1->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(sub1);
    QWidget *detW = new QWidget();
    QFormLayout *f = new QFormLayout(detW); f->setContentsMargins(0, 15, 0, 30); f->setVerticalSpacing(15);
    QString vs = "color: white; font-size: 15px;";
    lblNameVal = new QLabel("N/A"); lblNameVal->setStyleSheet(vs);
    lblEmailVal = new QLabel("N/A"); lblEmailVal->setStyleSheet(vs);
    lblPhoneVal = new QLabel("N/A"); lblPhoneVal->setStyleSheet(vs);
    lblDobVal = new QLabel("N/A"); lblDobVal->setStyleSheet(vs);
    lblCountryVal = new QLabel("N/A"); lblCountryVal->setStyleSheet(vs);
    lblGenderVal = new QLabel("N/A"); lblGenderVal->setStyleSheet(vs);
    lblAddressVal = new QLabel("N/A"); lblAddressVal->setStyleSheet(vs);
    f->addRow(new QLabel("Name:"), lblNameVal); f->addRow(new QLabel("Email:"), lblEmailVal);
    f->addRow(new QLabel("Phone:"), lblPhoneVal); f->addRow(new QLabel("Birth Date:"), lblDobVal);
    f->addRow(new QLabel("Country:"), lblCountryVal); f->addRow(new QLabel("Gender:"), lblGenderVal);
    f->addRow(new QLabel("Address:"), lblAddressVal);
    ml->addWidget(detW);
    QLabel *sub2 = new QLabel("Your Booking History"); sub2->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(sub2);
    QScrollArea *hsa = new QScrollArea(); hsa->setWidgetResizable(true); hsa->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    QWidget *hc = new QWidget(); QVBoxLayout *hl = new QVBoxLayout(hc); hl->setAlignment(Qt::AlignTop);
    for (const auto &b : userBookings) {
        QFrame *fr = new QFrame(); fr->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 8px; border: 1px solid #334155; padding: 5px; }");
        QHBoxLayout *cl = new QHBoxLayout(fr); QVBoxLayout *inf = new QVBoxLayout();
        QLabel *hN = new QLabel(b.hotelName); hN->setStyleSheet("font-weight: bold; color: white; border: none; background: transparent;");
        QLabel *hD = new QLabel(b.dateRange); hD->setStyleSheet("color: #94a3b8; font-size: 12px; border: none; background: transparent;");
        inf->addWidget(hN); inf->addWidget(hD);
        QLabel *st = new QLabel(b.status);
        st->setStyleSheet(b.status == "Finished" ? "color:#22c55e; border: none; background: transparent;" : "color:#3b82f6; border: none; background: transparent;");
        cl->addLayout(inf); cl->addStretch(); cl->addWidget(st); hl->addWidget(fr);
    }
    hsa->setWidget(hc); ml->addWidget(hsa);
    return w;
}

QWidget* MainWindow::createDetailsWidget() {
    QWidget *w = new QWidget();
    QVBoxLayout *ml = new QVBoxLayout(w);
    ml->setContentsMargins(30, 30, 30, 30);
    QHBoxLayout *h = new QHBoxLayout();
    QPushButton *bb = new QPushButton("← Back"); bb->setStyleSheet(secondaryBtnStyle);
    connect(bb, &QPushButton::clicked, this, &MainWindow::backToMainApp);
    detName = new QLabel("Hotel Details"); detName->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    h->addWidget(bb); h->addSpacing(20); h->addWidget(detName); h->addStretch();
    ml->addLayout(h);
    detAddress = new QLabel(""); detAddress->setStyleSheet("color: #94a3b8; font-size: 14px; margin-left: 85px;");
    ml->addWidget(detAddress);
    detPromo = new QLabel(""); ml->addWidget(detPromo);

    QHBoxLayout *filtersLayout = new QHBoxLayout();
    QLabel *filterLbl = new QLabel("Facilities:");
    filterLbl->setStyleSheet("color: #94a3b8; font-weight: bold; font-size: 14px;");
    filtersLayout->addWidget(filterLbl);

    cbBalcony = new QCheckBox("Balcony"); cbBalcony->setStyleSheet(checkBoxStyle);
    cbFridge = new QCheckBox("Fridge"); cbFridge->setStyleSheet(checkBoxStyle);
    cbAC = new QCheckBox("AC"); cbAC->setStyleSheet(checkBoxStyle);
    cbTV = new QCheckBox("TV"); cbTV->setStyleSheet(checkBoxStyle);
    cbWifi = new QCheckBox("WiFi"); cbWifi->setStyleSheet(checkBoxStyle);
    cbSofa = new QCheckBox("Sofa"); cbSofa->setStyleSheet(checkBoxStyle);

    filtersLayout->addWidget(cbBalcony); filtersLayout->addWidget(cbFridge);
    filtersLayout->addWidget(cbAC); filtersLayout->addWidget(cbTV);
    filtersLayout->addWidget(cbWifi); filtersLayout->addWidget(cbSofa);
    filtersLayout->addStretch();

    ml->addSpacing(10);
    ml->addLayout(filtersLayout);

    auto updateRoomFilters = [this]() { displayRooms(roomSearchBar->text()); };
    connect(cbBalcony, &QCheckBox::stateChanged, this, updateRoomFilters);
    connect(cbFridge, &QCheckBox::stateChanged, this, updateRoomFilters);
    connect(cbAC, &QCheckBox::stateChanged, this, updateRoomFilters);
    connect(cbTV, &QCheckBox::stateChanged, this, updateRoomFilters);
    connect(cbWifi, &QCheckBox::stateChanged, this, updateRoomFilters);
    connect(cbSofa, &QCheckBox::stateChanged, this, updateRoomFilters);

    roomSearchBar = new QLineEdit();
    roomSearchBar->setPlaceholderText("Filter rooms by text...");
    roomSearchBar->setStyleSheet(lineEditStyle);
    connect(roomSearchBar, &QLineEdit::textChanged, this, &MainWindow::filterRooms);
    ml->addSpacing(10); ml->addWidget(roomSearchBar);
    ml->addSpacing(20);
    QLabel *roomT = new QLabel("Available Rooms"); roomT->setStyleSheet("font-size: 18px; font-weight: bold; color: #3b82f6;");
    ml->addWidget(roomT);
    QScrollArea *sa = new QScrollArea(); sa->setWidgetResizable(true); sa->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    QWidget *rc = new QWidget(); roomsLayout = new QVBoxLayout(rc); roomsLayout->setAlignment(Qt::AlignTop);
    sa->setWidget(rc); ml->addWidget(sa);
    return w;
}

void MainWindow::openAccommodationDetails(const Accommodation &acc) {
    currentAccommodationInDetails = acc;
    detName->setText(acc.name);
    detAddress->setText("📍 " + acc.address + ", " + acc.location);
    if (roomSearchBar) roomSearchBar->clear();
    if (cbBalcony) cbBalcony->setChecked(false);
    if (cbFridge) cbFridge->setChecked(false);
    if (cbAC) cbAC->setChecked(false);
    if (cbTV) cbTV->setChecked(false);
    if (cbWifi) cbWifi->setChecked(false);
    if (cbSofa) cbSofa->setChecked(false);

    if (acc.discountPercent > 0) {
        detPromo->setText("🔥 Special Offer: " + acc.promoName + " (" + QString::number(acc.discountPercent * 100) + "% OFF!)");
        detPromo->setStyleSheet("color: #f59e0b; font-weight: bold; margin-left: 85px; font-size: 14px;");
        detPromo->show();
    } else { detPromo->hide(); }
    displayRooms("");
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::filterRooms(const QString &query) { displayRooms(query); }

void MainWindow::displayRooms(const QString &f) {
    QLayoutItem *child;
    while ((child = roomsLayout->takeAt(0)) != nullptr) { if(child->widget()) delete child->widget(); delete child; }

    bool reqBalcony = cbBalcony && cbBalcony->isChecked();
    bool reqFridge = cbFridge && cbFridge->isChecked();
    bool reqAC = cbAC && cbAC->isChecked();
    bool reqTV = cbTV && cbTV->isChecked();
    bool reqWifi = cbWifi && cbWifi->isChecked();
    bool reqSofa = cbSofa && cbSofa->isChecked();

    for (const auto &room : currentAccommodationInDetails.rooms) {
        bool matchesType = room.type.contains(f, Qt::CaseInsensitive);
        bool matchesFac = false;
        bool rBalcony = false, rFridge = false, rAC = false, rTV = false, rWifi = false;

        for (const auto &fac : room.facilities) {
            if(fac.name.contains(f, Qt::CaseInsensitive)) { matchesFac = true; }
            if (fac.name == "Balcony") rBalcony = true;
            if (fac.name == "Fridge") rFridge = true;
            if (fac.name == "AC") rAC = true;
            if (fac.name == "TV") rTV = true;
            if (fac.name == "WiFi") rWifi = true;
        }

        if (!f.isEmpty() && !matchesType && !matchesFac) continue;
        if (reqBalcony && !rBalcony) continue;
        if (reqFridge && !rFridge) continue;
        if (reqAC && !rAC) continue;
        if (reqTV && !rTV) continue;
        if (reqWifi && !rWifi) continue;
        if (reqSofa && !room.hasSofa) continue;

        QFrame *fr = new QFrame(); fr->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 10px; border: 1px solid #334155; }");
        QHBoxLayout *l = new QHBoxLayout(fr); l->setContentsMargins(15, 15, 15, 15);
        QVBoxLayout *inf = new QVBoxLayout();
        QLabel *typeL = new QLabel(room.type); typeL->setStyleSheet("font-size: 18px; font-weight: bold; color: white; border: none; background: transparent;");
        int totalCap = room.beds + (room.hasSofa ? 1 : 0);
        QLabel *capL = new QLabel(QString("🛏️ %1 Beds | 👥 Max Capacity: %2 people").arg(room.beds).arg(totalCap));
        capL->setStyleSheet("color: #94a3b8; border: none; background: transparent;");
        QString flist = "Facilities: ";
        for (const auto &fac : room.facilities) { flist += fac.name + ", "; }
        QLabel *facL = new QLabel(flist.left(flist.length() - 2)); facL->setStyleSheet("color: #64748b; font-size: 12px; border: none; background: transparent;");
        inf->addWidget(typeL); inf->addWidget(capL); inf->addWidget(facL);
        double finalPrice = room.basePrice * (1.0 - currentAccommodationInDetails.discountPercent);
        QLabel *pL = new QLabel(QString::number(finalPrice) + "€ / night");
        pL->setStyleSheet("color: #22c55e; font-size: 18px; font-weight: bold; border: none; background: transparent;");
        l->addLayout(inf); l->addStretch(); l->addWidget(pL);
        roomsLayout->addWidget(fr);
    }
}

void MainWindow::populateAccommodations(const QString &f) {
    QLayoutItem *c;
    while ((c = accommodationsLayout->takeAt(0)) != nullptr) { if(c->widget()) delete c->widget(); delete c; }
    for (const auto &acc : allAccommodations) {
        if (!f.isEmpty() && !acc.name.contains(f, Qt::CaseInsensitive) && !acc.location.contains(f, Qt::CaseInsensitive)) continue;
        QFrame *card = new QFrame(); card->setStyleSheet("QFrame { background-color: #1e293b; border-radius: 12px; }");
        QHBoxLayout *l = new QHBoxLayout(card); l->setContentsMargins(20, 20, 20, 20);
        QVBoxLayout *inf = new QVBoxLayout();
        QLabel *nL = new QLabel(acc.name); nL->setStyleSheet("color: white; font-size: 18px; font-weight: bold; border: none; background: transparent;");
        QLabel *lL = new QLabel("📍 " + acc.location); lL->setStyleSheet("color: #94a3b8; font-size: 14px; border: none; background: transparent;");
        inf->addWidget(nL); inf->addWidget(lL);
        QPushButton *btn = new QPushButton("View Details"); btn->setStyleSheet(primaryBtnStyle);
        connect(btn, &QPushButton::clicked, this, [this, acc]() { openAccommodationDetails(acc); });
        l->addLayout(inf); l->addStretch(); l->addWidget(btn);
        accommodationsLayout->addWidget(card);
    }
}

void MainWindow::goToRegister() { stackedWidget->setCurrentIndex(1); }
void MainWindow::goToLogin() {
    clearRegisterFields(); if (searchBarInput) searchBarInput->clear();
    stackedWidget->setCurrentIndex(0);
}
void MainWindow::goToUserProfile() { stackedWidget->setCurrentIndex(3); }
void MainWindow::backToMainApp() { stackedWidget->setCurrentIndex(2); }
void MainWindow::filterAccommodations(const QString &q) { populateAccommodations(q); }

void MainWindow::clearRegisterFields() {
    regNameInput->clear(); regEmailInput->clear(); regPasswordInput->clear();
    regPhoneInput->clear(); regAddressInput->clear();
    regDobInput->setDate(QDate(2000, 1, 1));
    regCountryInput->setCurrentIndex(0); regGenderInput->setCurrentIndex(0);
}