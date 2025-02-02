#include <QApplication>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFontDatabase>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>

#include <sstream>

#include "pre-definition.hpp"
#include "thread_pool.hpp"
#include "magic_enum/magic_enum.hpp"

enum ButtonAction : uint8_t
{
    LOAD,
    RELEASE,
    REMOVE,
    QUIT
};

template <typename T>
QString EnumToQString(T value)
{
    return QString("%1").arg(magic_enum::enum_name<T>(value).data());
}

inline void toupper_str(std::string& str)
{
    for (auto& cha : str)
    {
        cha = static_cast<char>(toupper(cha));
    }
}

inline void tolower_str(std::string& str)
{
    for (auto& cha : str)
    {
        cha = static_cast<char>(tolower(cha));
    }
}

class ModuleAction : public QAction
{
    Q_OBJECT
public:
    explicit ModuleAction(QWidget* parent = Q_NULLPTR) : QAction{parent}
    {
        connect(this, &ModuleAction::triggered, this, &ModuleAction::WhenTriggerd);
    }

    explicit ModuleAction(const QString& title, QWidget* parent = Q_NULLPTR) : QAction{title, parent}
    {
        connect(this, &ModuleAction::triggered, this, &ModuleAction::WhenTriggerd);
    }

private slots:

    void WhenTriggerd()
    {
        QObject* oaction = sender();
        if (oaction != nullptr)
        {
            auto* action = qobject_cast<QAction*>(oaction);
            if (action != nullptr && action->parent() != nullptr)
            {
                auto* sharedmenu = qobject_cast<QMenu*>(action->parent());
                if (sharedmenu != nullptr)
                {
                    const auto strtask = sharedmenu->property("source-action").toString().toStdString();
                    const auto task = magic_enum::enum_cast<ButtonAction>(strtask);
                    if (task.has_value())
                    {
                        auto moduleid = action->text().toStdString();
                        tolower_str(moduleid);

                        switch (task.value())
                        {
                        case ButtonAction::LOAD:
                        {

                            qDebug() << "Load Module triggered the menu action " << action->text();
                            if (ModuleManager::getInstance()->loadModule(moduleid))
                            {
                                auto mdinterface = ModuleManager::getInstance()->getModuleInstance(moduleid);
                                ThreadPool::getInstance()->submit(100, [mdinterface](){mdinterface->execute();});
                            }
                        }
                        break;
                        case ButtonAction::RELEASE:
                        {
                            qDebug() << "Release Module triggered the menu action " << action->text();
                            ModuleManager::getInstance()->releaseModuleLib(moduleid);
                        }
                        break;
                        case ButtonAction::REMOVE:
                        {
                            qDebug() << "Remove Instance triggered the menu action " << action->text();
                            ModuleManager::getInstance()->releaseModuleInstance(moduleid);
                        }
                        break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
};

class SharedMenu : public QMenu
{
    Q_OBJECT
private:
    void defaultInit()
    {
        connect(this, &SharedMenu::aboutToShow, this, &SharedMenu::centerMenu);
        setStyleSheet("QMenu {"
                      "    background-color: white;" // Menu background color
                      "    border: 1px solid black;" // Menu border style
                      "    padding: 5px;"            // Padding around the menu
                      "}"
                      "QMenu::item {"
                      "    padding: 10px 20px;"            // Padding for menu items
                      "    background-color: transparent;" // Default background for items
                      "    color: black;"                  // Text color for items
                      "}"
                      "QMenu::item:selected {"
                      "    background-color: lightblue;" // Background when hovering over an item
                      "    color: white;"                // Text color when hovering
                      "}");
    }

public:
    explicit SharedMenu(QWidget* parent = Q_NULLPTR) : QMenu{parent}
    {
        defaultInit();
    }

    explicit SharedMenu(const QString& title, QWidget* parent = Q_NULLPTR) : QMenu{title, parent}
    {
        defaultInit();
    }

private slots:

    void centerMenu()
    {

        QTimer::singleShot(0, [this]() {
            const QRect srcRect = property("source-rect").toRect();
            move(this->pos() + QPoint(srcRect.width() + 100, -srcRect.height()));
        });
    }
};

class FunctionalButton : public QPushButton
{
public:
    explicit FunctionalButton(const QString& text, QWidget* parent = (QWidget*)nullptr) : QPushButton(text, parent)
    {
        setFixedHeight(126);
        setMaximumWidth(500);
        setMinimumWidth(366);
        setStyleSheet("QPushButton {"
                      "   font-family: 'Ubuntu';"     // Set font family
                      "   font-size: 32px;"           // Font size
                      "   font-weight: bold;"         // Bold text
                      "   color: #AD4006;"            // Text color
                      "   border: 2px solid black;"   // Border style
                      "   border-radius: 15px;"       // Rounded corners (15px radius)
                      "   background-color: #4EB977;" // Background color
                      "   padding: 20px;"             // Padding inside the button
                      "}"
                      "QPushButton:hover {"
                      "   background-color: #A0DEB8;" // Background color on hover
                      "}"
                      "QPushButton:pressed {"
                      "   background-color: #759983;" // Background color when pressed
                      "}");
        auto buttonPressed = [this]() {
            QObject* button = sender();
            if (button)
            {
                const QPushButton* senderButton = qobject_cast<QPushButton*>(button);
                if (senderButton)
                {
                    auto task = magic_enum::enum_cast<ButtonAction>(senderButton->objectName().toStdString());
                    if (task.has_value() && task.value() == ButtonAction::QUIT)
                    {
                        qDebug() << "Quit";
                        QApplication::quit();
                        return;
                    }
                    auto* sharedMenu = senderButton->menu();
                    sharedMenu->setProperty("source-action", senderButton->objectName());
                    sharedMenu->setProperty("source-position", senderButton->pos());
                    sharedMenu->setProperty("source-rect", senderButton->rect());
                }
            }
        };
        connect(this, &QPushButton::pressed, this, buttonPressed);
    }
};

class ModuleButton : public QPushButton
{
public:
    explicit ModuleButton(const QString& text, QWidget* parent = (QWidget*)nullptr) : QPushButton(text, parent)
    {
        setFixedHeight(240);
        setMinimumWidth(490);
        setMaximumWidth(600);
    }
};

class HandComputer : public QMainWindow
{
    Q_OBJECT

public:
    HandComputer()
    {
        auto* centralWidget = new QWidget(this);
        auto* mainLayout = new QHBoxLayout(centralWidget);

        mainLayout->setContentsMargins(92, 92, 92, 92);

        auto* leftWidget = new QWidget();
        auto* leftLayout = new QVBoxLayout(leftWidget);
        leftLayout->setContentsMargins(20, 20, 20, 20);

        // Create a single shared QMenu instance
        auto* sharedMenu = new SharedMenu();

        // Add actions to the menu
        QList<QAction*> actions;
        auto moduleIDs = magic_enum::enum_names<ModuleName>();
        for (auto& moduleID : moduleIDs)
        {
            std::string strID{moduleID.data(), moduleID.length()};
            auto* action = new ModuleAction(QString::fromStdString(strID), sharedMenu);
            actions.append(action);
        }
        sharedMenu->addActions(actions);

        // Create multiple buttons
        QPushButton* loadModuleButton = new FunctionalButton("Load Module");
        loadModuleButton->setObjectName(EnumToQString(ButtonAction::LOAD));
        QPushButton* releaseModuleButton = new FunctionalButton("Release Module");
        releaseModuleButton->setObjectName(EnumToQString(ButtonAction::RELEASE));
        QPushButton* removeInstanceButton = new FunctionalButton("Remove Instance");
        removeInstanceButton->setObjectName(EnumToQString(ButtonAction::REMOVE));
        QPushButton* quitButton = new FunctionalButton("Quit");
        quitButton->setObjectName(EnumToQString(ButtonAction::QUIT));

        leftLayout->addWidget(loadModuleButton);
        leftLayout->addStretch();
        leftLayout->addWidget(releaseModuleButton);
        leftLayout->addStretch();
        leftLayout->addWidget(removeInstanceButton);
        leftLayout->addStretch();
        leftLayout->addWidget(quitButton);

        // Assign the shared menu to each button
        loadModuleButton->setMenu(sharedMenu);
        releaseModuleButton->setMenu(sharedMenu);
        removeInstanceButton->setMenu(sharedMenu);

        leftWidget->setFixedWidth(406);

        auto* rightWidget = new QWidget();
        auto* rightLayout = new QVBoxLayout(rightWidget);
        rightLayout->setContentsMargins(20, 20, 20, 20);
        rightLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        rightLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

        QPushButton* calculatorButton = new ModuleButton("Calculator");
        QPushButton* translatorButton = new ModuleButton("Translator");
        QPushButton* gamesButton = new ModuleButton("Games");

        rightLayout->addWidget(calculatorButton);
        rightLayout->addStretch();
        rightLayout->addWidget(translatorButton);
        rightLayout->addStretch();
        rightLayout->addWidget(gamesButton);

        rightWidget->setFixedWidth(640);

        mainLayout->addWidget(leftWidget);
        mainLayout->addWidget(rightWidget);

        // Set central widget
        setCentralWidget(centralWidget);

        // Set window properties
        setWindowTitle("Hand Computer");
        resize(1920, 1080); // Adjust window size
    }
};

int main(int argc, char* argv[])
{
    init();
    QApplication app(argc, argv);
    // const QStringList fontFamilies = QFontDatabase().families();
    // for (const QString& font : fontFamilies)
    // {
    //     qDebug() << font;
    // }
    HandComputer window;
    window.show(); // Show maximized but with borders
    return app.exec();
}

#include "main.moc"
