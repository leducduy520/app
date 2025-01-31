#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

class HandComputer : public QMainWindow {
    Q_OBJECT

public:
    HandComputer() {
        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        // Title Label
        QLabel *title = new QLabel("Hand Computer", this);
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 24px; font-weight: bold;");
        layout->addWidget(title);

        // Buttons
        QPushButton *calculatorButton = new QPushButton("Calculator", this);
        QPushButton *translatorButton = new QPushButton("Translator", this);
        QPushButton *gamesButton = new QPushButton("Games", this);

        // Style buttons
        QString buttonStyle = "font-size: 18px; padding: 10px;";
        calculatorButton->setStyleSheet(buttonStyle);
        translatorButton->setStyleSheet(buttonStyle);
        gamesButton->setStyleSheet(buttonStyle);

        layout->addWidget(calculatorButton);
        layout->addWidget(translatorButton);
        layout->addWidget(gamesButton);

        // Connect button signals to slots
        connect(calculatorButton, &QPushButton::clicked, this, &HandComputer::launchCalculator);
        connect(translatorButton, &QPushButton::clicked, this, &HandComputer::launchTranslator);
        connect(gamesButton, &QPushButton::clicked, this, &HandComputer::launchGames);

        // Set central widget and full-screen
        setCentralWidget(centralWidget);
        this->showMaximized();
        setWindowTitle("Hand Computer");
    }

private:
    void launchCalculator() {
        QMessageBox::information(this, "Calculator", "Launching Calculator Module...");
        // Add your logic to launch the calculator module here
    }

    void launchTranslator() {
        QMessageBox::information(this, "Translator", "Launching Translator Module...");
        // Add your logic to launch the translator module here
    }

    void launchGames() {
        QMessageBox::information(this, "Games", "Launching Games Module...");
        // Add your logic to launch the games module here
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HandComputer window;
    window.show();
    return app.exec();
}

#include "main.moc"
