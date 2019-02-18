/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include "src/qt/colorwheel.h"
#include "src/qt/transferfunctionwidget.h"
#include "src/qt/volumerenderwidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionQuit;
    QAction *actionSaveRawTff;
    QAction *actionSettings;
    QAction *actionLoadRawTff;
    QAction *actionLoadCpTff;
    QAction *actionSaveRawTff_2;
    QAction *actionSaveCpTff;
    QAction *actionGenerateLowResVo;
    QAction *actionScreenshot;
    QAction *actionRecord;
    QAction *actionResetCam;
    QAction *actionSaveState;
    QAction *actionLoadState;
    QAction *actionShowOverlay;
    QAction *actionFullscreen;
    QAction *actionShowStatusBar;
    QAction *actionSelectOpenCL;
    QAction *actionAbout;
    QAction *actionRealoadKernel;
    QAction *actionRecordCamera;
    QAction *actionLogInteraction;
    QAction *actionSelectEyetrackingDevice;
    QAction *actionSelectMonitor;
    QAction *actionLoad_Index_and_Sampling_Map;
    QAction *actionLoad_Sampling_Map;
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    VolumeRenderWidget *volumeRenderWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuLoad_transfer_function;
    QMenu *menuSave_transfer_function;
    QMenu *menuView;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QMenu *menuEyetracking_Settings;
    QMenu *menuLBG_Sampling;
    QStatusBar *statusBar;
    QDockWidget *dockSettings;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout_2;
    QGroupBox *gbTimeSeries;
    QGridLayout *gridLayout_4;
    QPushButton *pbPlay;
    QSlider *sldTimeStep;
    QCheckBox *chbLoop;
    QSpinBox *sbTimeStep;
    QLabel *label_3;
    QSpinBox *sbSpeed;
    QComboBox *cbInterpolation;
    colorwidgets::ColorWheel *colorWheel;
    TransferFunctionWidget *transferFunctionEditor;
    QPushButton *pbResetTff;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QComboBox *cbIllum;
    QDoubleSpinBox *dsbImgSampling;
    QCheckBox *chbLinear;
    QCheckBox *chbOrtho;
    QCheckBox *chbObjectESS;
    QCheckBox *chbBox;
    QDoubleSpinBox *dsbSamplingRate;
    QCheckBox *chbContours;
    QLabel *label_4;
    QCheckBox *chbAerial;
    QLabel *label_2;
    QPushButton *pbBgColor;
    QCheckBox *chbAmbientOcclusion;
    QCheckBox *chbContRendering;
    QCheckBox *chbImageESS;
    QLabel *label;
    QComboBox *setRdMt;
    QCheckBox *chbEyetracking;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1752, 1179);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon;
        QString iconThemeName = QStringLiteral("document-open");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionOpen->setIcon(icon);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        QIcon icon1;
        iconThemeName = QStringLiteral("application-exit");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon1 = QIcon::fromTheme(iconThemeName);
        } else {
            icon1.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionQuit->setIcon(icon1);
        actionSaveRawTff = new QAction(MainWindow);
        actionSaveRawTff->setObjectName(QStringLiteral("actionSaveRawTff"));
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName(QStringLiteral("actionSettings"));
        actionSettings->setCheckable(true);
        actionSettings->setChecked(true);
        actionLoadRawTff = new QAction(MainWindow);
        actionLoadRawTff->setObjectName(QStringLiteral("actionLoadRawTff"));
        actionLoadCpTff = new QAction(MainWindow);
        actionLoadCpTff->setObjectName(QStringLiteral("actionLoadCpTff"));
        actionSaveRawTff_2 = new QAction(MainWindow);
        actionSaveRawTff_2->setObjectName(QStringLiteral("actionSaveRawTff_2"));
        actionSaveCpTff = new QAction(MainWindow);
        actionSaveCpTff->setObjectName(QStringLiteral("actionSaveCpTff"));
        actionGenerateLowResVo = new QAction(MainWindow);
        actionGenerateLowResVo->setObjectName(QStringLiteral("actionGenerateLowResVo"));
        actionScreenshot = new QAction(MainWindow);
        actionScreenshot->setObjectName(QStringLiteral("actionScreenshot"));
        QIcon icon2;
        iconThemeName = QStringLiteral("insert-image");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon2 = QIcon::fromTheme(iconThemeName);
        } else {
            icon2.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionScreenshot->setIcon(icon2);
        actionRecord = new QAction(MainWindow);
        actionRecord->setObjectName(QStringLiteral("actionRecord"));
        actionRecord->setCheckable(true);
        actionResetCam = new QAction(MainWindow);
        actionResetCam->setObjectName(QStringLiteral("actionResetCam"));
        QIcon icon3;
        iconThemeName = QStringLiteral("view-refresh");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon3 = QIcon::fromTheme(iconThemeName);
        } else {
            icon3.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionResetCam->setIcon(icon3);
        actionSaveState = new QAction(MainWindow);
        actionSaveState->setObjectName(QStringLiteral("actionSaveState"));
        actionLoadState = new QAction(MainWindow);
        actionLoadState->setObjectName(QStringLiteral("actionLoadState"));
        actionShowOverlay = new QAction(MainWindow);
        actionShowOverlay->setObjectName(QStringLiteral("actionShowOverlay"));
        actionShowOverlay->setCheckable(true);
        actionShowOverlay->setChecked(true);
        actionFullscreen = new QAction(MainWindow);
        actionFullscreen->setObjectName(QStringLiteral("actionFullscreen"));
        actionShowStatusBar = new QAction(MainWindow);
        actionShowStatusBar->setObjectName(QStringLiteral("actionShowStatusBar"));
        actionShowStatusBar->setCheckable(true);
        actionShowStatusBar->setChecked(true);
        actionSelectOpenCL = new QAction(MainWindow);
        actionSelectOpenCL->setObjectName(QStringLiteral("actionSelectOpenCL"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        QIcon icon4;
        iconThemeName = QStringLiteral("help-about");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon4 = QIcon::fromTheme(iconThemeName);
        } else {
            icon4.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        actionAbout->setIcon(icon4);
        actionRealoadKernel = new QAction(MainWindow);
        actionRealoadKernel->setObjectName(QStringLiteral("actionRealoadKernel"));
        actionRealoadKernel->setIcon(icon3);
        actionRecordCamera = new QAction(MainWindow);
        actionRecordCamera->setObjectName(QStringLiteral("actionRecordCamera"));
        actionRecordCamera->setCheckable(true);
        actionLogInteraction = new QAction(MainWindow);
        actionLogInteraction->setObjectName(QStringLiteral("actionLogInteraction"));
        actionLogInteraction->setCheckable(true);
        actionSelectEyetrackingDevice = new QAction(MainWindow);
        actionSelectEyetrackingDevice->setObjectName(QStringLiteral("actionSelectEyetrackingDevice"));
        actionSelectMonitor = new QAction(MainWindow);
        actionSelectMonitor->setObjectName(QStringLiteral("actionSelectMonitor"));
        actionLoad_Index_and_Sampling_Map = new QAction(MainWindow);
        actionLoad_Index_and_Sampling_Map->setObjectName(QStringLiteral("actionLoad_Index_and_Sampling_Map"));
        actionLoad_Sampling_Map = new QAction(MainWindow);
        actionLoad_Sampling_Map->setObjectName(QStringLiteral("actionLoad_Sampling_Map"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        volumeRenderWidget = new VolumeRenderWidget(centralWidget);
        volumeRenderWidget->setObjectName(QStringLiteral("volumeRenderWidget"));

        gridLayout_3->addWidget(volumeRenderWidget, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1752, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuLoad_transfer_function = new QMenu(menuFile);
        menuLoad_transfer_function->setObjectName(QStringLiteral("menuLoad_transfer_function"));
        menuSave_transfer_function = new QMenu(menuFile);
        menuSave_transfer_function->setObjectName(QStringLiteral("menuSave_transfer_function"));
        QIcon icon5;
        iconThemeName = QStringLiteral("document-save-as");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon5 = QIcon::fromTheme(iconThemeName);
        } else {
            icon5.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        menuSave_transfer_function->setIcon(icon5);
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuEyetracking_Settings = new QMenu(menuBar);
        menuEyetracking_Settings->setObjectName(QStringLiteral("menuEyetracking_Settings"));
        menuLBG_Sampling = new QMenu(menuBar);
        menuLBG_Sampling->setObjectName(QStringLiteral("menuLBG_Sampling"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        dockSettings = new QDockWidget(MainWindow);
        dockSettings->setObjectName(QStringLiteral("dockSettings"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(dockSettings->sizePolicy().hasHeightForWidth());
        dockSettings->setSizePolicy(sizePolicy);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        gridLayout_2 = new QGridLayout(dockWidgetContents);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gbTimeSeries = new QGroupBox(dockWidgetContents);
        gbTimeSeries->setObjectName(QStringLiteral("gbTimeSeries"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(gbTimeSeries->sizePolicy().hasHeightForWidth());
        gbTimeSeries->setSizePolicy(sizePolicy1);
        gbTimeSeries->setMinimumSize(QSize(0, 0));
        gridLayout_4 = new QGridLayout(gbTimeSeries);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        pbPlay = new QPushButton(gbTimeSeries);
        pbPlay->setObjectName(QStringLiteral("pbPlay"));
        QIcon icon6;
        iconThemeName = QStringLiteral("media-playback-start");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon6 = QIcon::fromTheme(iconThemeName);
        } else {
            icon6.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        pbPlay->setIcon(icon6);
        pbPlay->setFlat(false);

        gridLayout_4->addWidget(pbPlay, 2, 1, 1, 1);

        sldTimeStep = new QSlider(gbTimeSeries);
        sldTimeStep->setObjectName(QStringLiteral("sldTimeStep"));
        sldTimeStep->setPageStep(1);
        sldTimeStep->setOrientation(Qt::Horizontal);

        gridLayout_4->addWidget(sldTimeStep, 0, 1, 1, 3);

        chbLoop = new QCheckBox(gbTimeSeries);
        chbLoop->setObjectName(QStringLiteral("chbLoop"));
        chbLoop->setChecked(true);

        gridLayout_4->addWidget(chbLoop, 2, 2, 1, 1);

        sbTimeStep = new QSpinBox(gbTimeSeries);
        sbTimeStep->setObjectName(QStringLiteral("sbTimeStep"));

        gridLayout_4->addWidget(sbTimeStep, 0, 4, 1, 1);

        label_3 = new QLabel(gbTimeSeries);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_4->addWidget(label_3, 2, 3, 1, 1);

        sbSpeed = new QSpinBox(gbTimeSeries);
        sbSpeed->setObjectName(QStringLiteral("sbSpeed"));
        sbSpeed->setMinimum(1);
        sbSpeed->setMaximum(1000);
        sbSpeed->setSingleStep(10);
        sbSpeed->setValue(100);

        gridLayout_4->addWidget(sbSpeed, 2, 4, 1, 1);


        gridLayout_2->addWidget(gbTimeSeries, 1, 0, 1, 2);

        cbInterpolation = new QComboBox(dockWidgetContents);
        cbInterpolation->addItem(QString());
        cbInterpolation->addItem(QString());
        cbInterpolation->setObjectName(QStringLiteral("cbInterpolation"));

        gridLayout_2->addWidget(cbInterpolation, 4, 0, 1, 1);

        colorWheel = new colorwidgets::ColorWheel(dockWidgetContents);
        colorWheel->setObjectName(QStringLiteral("colorWheel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(colorWheel->sizePolicy().hasHeightForWidth());
        colorWheel->setSizePolicy(sizePolicy2);
        colorWheel->setMaximumSize(QSize(16777215, 200));

        gridLayout_2->addWidget(colorWheel, 2, 0, 1, 2);

        transferFunctionEditor = new TransferFunctionWidget(dockWidgetContents);
        transferFunctionEditor->setObjectName(QStringLiteral("transferFunctionEditor"));

        gridLayout_2->addWidget(transferFunctionEditor, 3, 0, 1, 2);

        pbResetTff = new QPushButton(dockWidgetContents);
        pbResetTff->setObjectName(QStringLiteral("pbResetTff"));

        gridLayout_2->addWidget(pbResetTff, 4, 1, 1, 1);

        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy3);
        groupBox->setMaximumSize(QSize(16777215, 250));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setVerticalSpacing(4);
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 5, 0, 1, 2);

        cbIllum = new QComboBox(groupBox);
        cbIllum->addItem(QString());
        cbIllum->addItem(QString());
        cbIllum->addItem(QString());
        cbIllum->addItem(QString());
        cbIllum->addItem(QString());
        cbIllum->addItem(QString());
        cbIllum->setObjectName(QStringLiteral("cbIllum"));

        gridLayout->addWidget(cbIllum, 6, 0, 1, 5);

        dsbImgSampling = new QDoubleSpinBox(groupBox);
        dsbImgSampling->setObjectName(QStringLiteral("dsbImgSampling"));
        dsbImgSampling->setMinimum(0.01);
        dsbImgSampling->setMaximum(8);
        dsbImgSampling->setSingleStep(0.01);
        dsbImgSampling->setValue(1);

        gridLayout->addWidget(dsbImgSampling, 3, 1, 1, 1);

        chbLinear = new QCheckBox(groupBox);
        chbLinear->setObjectName(QStringLiteral("chbLinear"));
        chbLinear->setChecked(true);

        gridLayout->addWidget(chbLinear, 8, 0, 1, 2);

        chbOrtho = new QCheckBox(groupBox);
        chbOrtho->setObjectName(QStringLiteral("chbOrtho"));

        gridLayout->addWidget(chbOrtho, 11, 0, 1, 2);

        chbObjectESS = new QCheckBox(groupBox);
        chbObjectESS->setObjectName(QStringLiteral("chbObjectESS"));
        chbObjectESS->setChecked(true);

        gridLayout->addWidget(chbObjectESS, 13, 0, 1, 2);

        chbBox = new QCheckBox(groupBox);
        chbBox->setObjectName(QStringLiteral("chbBox"));
        chbBox->setChecked(false);

        gridLayout->addWidget(chbBox, 14, 0, 1, 3);

        dsbSamplingRate = new QDoubleSpinBox(groupBox);
        dsbSamplingRate->setObjectName(QStringLiteral("dsbSamplingRate"));
        dsbSamplingRate->setDecimals(1);
        dsbSamplingRate->setMinimum(0.1);
        dsbSamplingRate->setMaximum(20);
        dsbSamplingRate->setSingleStep(0.1);
        dsbSamplingRate->setValue(1.5);

        gridLayout->addWidget(dsbSamplingRate, 3, 3, 1, 2);

        chbContours = new QCheckBox(groupBox);
        chbContours->setObjectName(QStringLiteral("chbContours"));
        chbContours->setChecked(false);

        gridLayout->addWidget(chbContours, 10, 0, 1, 2);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 2, 1, 1);

        chbAerial = new QCheckBox(groupBox);
        chbAerial->setObjectName(QStringLiteral("chbAerial"));

        gridLayout->addWidget(chbAerial, 10, 2, 1, 3);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        pbBgColor = new QPushButton(groupBox);
        pbBgColor->setObjectName(QStringLiteral("pbBgColor"));
        pbBgColor->setAutoFillBackground(false);
        pbBgColor->setStyleSheet(QLatin1String("QPushButton:default {\n"
"   background: yellow;\n"
"}"));

        gridLayout->addWidget(pbBgColor, 5, 2, 1, 3);

        chbAmbientOcclusion = new QCheckBox(groupBox);
        chbAmbientOcclusion->setObjectName(QStringLiteral("chbAmbientOcclusion"));
        chbAmbientOcclusion->setEnabled(true);
        chbAmbientOcclusion->setChecked(false);

        gridLayout->addWidget(chbAmbientOcclusion, 8, 2, 1, 3);

        chbContRendering = new QCheckBox(groupBox);
        chbContRendering->setObjectName(QStringLiteral("chbContRendering"));

        gridLayout->addWidget(chbContRendering, 11, 2, 1, 3);

        chbImageESS = new QCheckBox(groupBox);
        chbImageESS->setObjectName(QStringLiteral("chbImageESS"));

        gridLayout->addWidget(chbImageESS, 13, 2, 1, 2);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        setRdMt = new QComboBox(groupBox);
        setRdMt->addItem(QString());
        setRdMt->addItem(QString());
        setRdMt->setObjectName(QStringLiteral("setRdMt"));

        gridLayout->addWidget(setRdMt, 1, 0, 1, 2);

        chbEyetracking = new QCheckBox(groupBox);
        chbEyetracking->setObjectName(QStringLiteral("chbEyetracking"));

        gridLayout->addWidget(chbEyetracking, 1, 2, 1, 3);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 2);

        dockSettings->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockSettings);
        QWidget::setTabOrder(dsbImgSampling, dsbSamplingRate);
        QWidget::setTabOrder(dsbSamplingRate, pbBgColor);
        QWidget::setTabOrder(pbBgColor, cbIllum);
        QWidget::setTabOrder(cbIllum, sldTimeStep);
        QWidget::setTabOrder(sldTimeStep, sbTimeStep);
        QWidget::setTabOrder(sbTimeStep, cbInterpolation);
        QWidget::setTabOrder(cbInterpolation, pbResetTff);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuBar->addAction(menuEyetracking_Settings->menuAction());
        menuBar->addAction(menuLBG_Sampling->menuAction());
        menuFile->addAction(actionOpen);
        menuFile->addSeparator();
        menuFile->addAction(menuLoad_transfer_function->menuAction());
        menuFile->addAction(menuSave_transfer_function->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(actionLoadState);
        menuFile->addAction(actionSaveState);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuLoad_transfer_function->addAction(actionLoadRawTff);
        menuLoad_transfer_function->addAction(actionLoadCpTff);
        menuSave_transfer_function->addAction(actionSaveRawTff_2);
        menuSave_transfer_function->addAction(actionSaveCpTff);
        menuView->addAction(actionResetCam);
        menuView->addSeparator();
        menuView->addAction(actionSettings);
        menuView->addAction(actionShowOverlay);
        menuView->addAction(actionShowStatusBar);
        menuEdit->addAction(actionGenerateLowResVo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionScreenshot);
        menuEdit->addAction(actionRecord);
        menuEdit->addAction(actionRecordCamera);
        menuEdit->addAction(actionLogInteraction);
        menuEdit->addSeparator();
        menuEdit->addAction(actionSelectOpenCL);
        menuEdit->addAction(actionRealoadKernel);
        menuHelp->addAction(actionAbout);
        menuEyetracking_Settings->addAction(actionSelectEyetrackingDevice);
        menuEyetracking_Settings->addAction(actionSelectMonitor);
        menuLBG_Sampling->addAction(actionLoad_Index_and_Sampling_Map);

        retranslateUi(MainWindow);
        QObject::connect(actionQuit, SIGNAL(triggered()), MainWindow, SLOT(close()));
        QObject::connect(sldTimeStep, SIGNAL(valueChanged(int)), sbTimeStep, SLOT(setValue(int)));
        QObject::connect(sbTimeStep, SIGNAL(valueChanged(int)), sldTimeStep, SLOT(setValue(int)));
        QObject::connect(actionSettings, SIGNAL(toggled(bool)), dockSettings, SLOT(setVisible(bool)));
        QObject::connect(actionShowStatusBar, SIGNAL(toggled(bool)), statusBar, SLOT(setVisible(bool)));

        cbIllum->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Volume Raycaster", nullptr));
        actionOpen->setText(QApplication::translate("MainWindow", "Open volume dataset...", nullptr));
#ifndef QT_NO_SHORTCUT
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
#ifndef QT_NO_SHORTCUT
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_NO_SHORTCUT
        actionSaveRawTff->setText(QApplication::translate("MainWindow", "Save raw transfer functio...", nullptr));
        actionSettings->setText(QApplication::translate("MainWindow", "Settings", nullptr));
        actionLoadRawTff->setText(QApplication::translate("MainWindow", "From raw values...", nullptr));
#ifndef QT_NO_SHORTCUT
        actionLoadRawTff->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_NO_SHORTCUT
        actionLoadCpTff->setText(QApplication::translate("MainWindow", "From control points...", nullptr));
#ifndef QT_NO_SHORTCUT
        actionLoadCpTff->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", nullptr));
#endif // QT_NO_SHORTCUT
        actionSaveRawTff_2->setText(QApplication::translate("MainWindow", "As raw values...", nullptr));
        actionSaveCpTff->setText(QApplication::translate("MainWindow", "As control points...", nullptr));
        actionGenerateLowResVo->setText(QApplication::translate("MainWindow", "Generate low resolution volume...", nullptr));
#ifndef QT_NO_SHORTCUT
        actionGenerateLowResVo->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", nullptr));
#endif // QT_NO_SHORTCUT
        actionScreenshot->setText(QApplication::translate("MainWindow", "Save current screen", nullptr));
#ifndef QT_NO_SHORTCUT
        actionScreenshot->setShortcut(QApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_NO_SHORTCUT
        actionRecord->setText(QApplication::translate("MainWindow", "Record rendered images", nullptr));
#ifndef QT_NO_SHORTCUT
        actionRecord->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", nullptr));
#endif // QT_NO_SHORTCUT
        actionResetCam->setText(QApplication::translate("MainWindow", "Reset camera", nullptr));
#ifndef QT_NO_SHORTCUT
        actionResetCam->setShortcut(QApplication::translate("MainWindow", "Home", nullptr));
#endif // QT_NO_SHORTCUT
        actionSaveState->setText(QApplication::translate("MainWindow", "Save state...", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSaveState->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionLoadState->setText(QApplication::translate("MainWindow", "Load state...", nullptr));
#ifndef QT_NO_SHORTCUT
        actionLoadState->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_NO_SHORTCUT
        actionShowOverlay->setText(QApplication::translate("MainWindow", "Overlay", nullptr));
        actionFullscreen->setText(QApplication::translate("MainWindow", "Fullscreen", nullptr));
#ifndef QT_NO_SHORTCUT
        actionFullscreen->setShortcut(QApplication::translate("MainWindow", "F11", nullptr));
#endif // QT_NO_SHORTCUT
        actionShowStatusBar->setText(QApplication::translate("MainWindow", "Status bar", nullptr));
        actionSelectOpenCL->setText(QApplication::translate("MainWindow", "Select OpenCL platform/device...", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About...", nullptr));
        actionRealoadKernel->setText(QApplication::translate("MainWindow", "Reaload kernels", nullptr));
#ifndef QT_NO_SHORTCUT
        actionRealoadKernel->setShortcut(QApplication::translate("MainWindow", "Ctrl+K", nullptr));
#endif // QT_NO_SHORTCUT
        actionRecordCamera->setText(QApplication::translate("MainWindow", "Record camera configuration", nullptr));
        actionLogInteraction->setText(QApplication::translate("MainWindow", "Log interaction", nullptr));
        actionSelectEyetrackingDevice->setText(QApplication::translate("MainWindow", "Select Eyetracking Device", nullptr));
        actionSelectMonitor->setText(QApplication::translate("MainWindow", "Select Calibrated Monitor", nullptr));
        actionLoad_Index_and_Sampling_Map->setText(QApplication::translate("MainWindow", "Load Index and Sampling Map", nullptr));
        actionLoad_Sampling_Map->setText(QApplication::translate("MainWindow", "Load Sampling Map", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuLoad_transfer_function->setTitle(QApplication::translate("MainWindow", "Load transfer function", nullptr));
        menuSave_transfer_function->setTitle(QApplication::translate("MainWindow", "Save transfer function", nullptr));
        menuView->setTitle(QApplication::translate("MainWindow", "View", nullptr));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
        menuEyetracking_Settings->setTitle(QApplication::translate("MainWindow", "Eyetracking Settings", nullptr));
        menuLBG_Sampling->setTitle(QApplication::translate("MainWindow", "LBG Sampling", nullptr));
        dockSettings->setWindowTitle(QApplication::translate("MainWindow", "Settings", nullptr));
        gbTimeSeries->setTitle(QApplication::translate("MainWindow", "Time series", nullptr));
        pbPlay->setText(QString());
        chbLoop->setText(QApplication::translate("MainWindow", "Loop", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Speed [ms]", nullptr));
        cbInterpolation->setItemText(0, QApplication::translate("MainWindow", "Linear", nullptr));
        cbInterpolation->setItemText(1, QApplication::translate("MainWindow", "Quad", nullptr));

        pbResetTff->setText(QApplication::translate("MainWindow", "Reset", nullptr));
        groupBox->setTitle(QApplication::translate("MainWindow", "Rendering", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Shading", nullptr));
        cbIllum->setItemText(0, QApplication::translate("MainWindow", "Off", nullptr));
        cbIllum->setItemText(1, QApplication::translate("MainWindow", "Central differences", nullptr));
        cbIllum->setItemText(2, QApplication::translate("MainWindow", "Central differences & transfer function", nullptr));
        cbIllum->setItemText(3, QApplication::translate("MainWindow", "Sobel filter", nullptr));
        cbIllum->setItemText(4, QApplication::translate("MainWindow", "Magnitude", nullptr));
        cbIllum->setItemText(5, QApplication::translate("MainWindow", "Cel shading", nullptr));

        chbLinear->setText(QApplication::translate("MainWindow", "Interpolation", nullptr));
        chbOrtho->setText(QApplication::translate("MainWindow", "Orthographic", nullptr));
        chbObjectESS->setText(QApplication::translate("MainWindow", "Obejct ESS", nullptr));
        chbBox->setText(QApplication::translate("MainWindow", "Show skipped", nullptr));
        chbContours->setText(QApplication::translate("MainWindow", "Contours", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Ray", nullptr));
        chbAerial->setText(QApplication::translate("MainWindow", "Aerial perspective", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "Image", nullptr));
        pbBgColor->setText(QApplication::translate("MainWindow", "Background color", nullptr));
        chbAmbientOcclusion->setText(QApplication::translate("MainWindow", "Ambient occlusion", nullptr));
        chbContRendering->setText(QApplication::translate("MainWindow", "Continued rendering", nullptr));
        chbImageESS->setText(QApplication::translate("MainWindow", "Image ESS", nullptr));
        label->setText(QApplication::translate("MainWindow", "Sampling rate", nullptr));
        setRdMt->setItemText(0, QApplication::translate("MainWindow", "Standard", nullptr));
        setRdMt->setItemText(1, QApplication::translate("MainWindow", "LBG-Sampling", nullptr));

        chbEyetracking->setText(QApplication::translate("MainWindow", "Use Eyetracking", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
