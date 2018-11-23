/*
 *      This is an interactive demo application for the algorithm proposed in:
 *
 *      Weighted Linde-Buzo Gray Stippling
 *      Oliver Deussen, Marc Spicker, Qian Zheng
 *
 *      In: ACM Transactions on Graphics (Proceedings of SIGGRAPH Asia 2017)
 *      https://doi.org/10.1145/3130800.3130819
 *
 *     Copyright 2017 Marc Spicker (marc.spicker@googlemail.com)
 */

#include <QApplication>
#include <QSvgGenerator>
#include <cassert>

#include "lbgstippling.h"
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Weighted Linde-Buzo-Gray Stippling");

    QCommandLineParser parser;
    parser.setApplicationDescription("This is a helping text.");
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    parser.addHelpOption();

    const char* c = "main";
    parser.addOptions({{"input", QCoreApplication::translate(c, "Input image."),
                        QCoreApplication::translate(c, "image")},
                       {"ip", QCoreApplication::translate(c, "Initial number of points."),
                        QCoreApplication::translate(c, "points")},
                       {"ips", QCoreApplication::translate(c, "Initial point size."),
                        QCoreApplication::translate(c, "size")},
                       {"cps", QCoreApplication::translate(c, "Use constant point size.")},
                       {"psmin", QCoreApplication::translate(c, "Minimum point size."),
                        QCoreApplication::translate(c, "size")},
                       {"psmax", QCoreApplication::translate(c, "Maximum point size."),
                        QCoreApplication::translate(c, "size")},
                       {"psmapping", QCoreApplication::translate(c, "Point size mapping."),
                        QCoreApplication::translate(c, "mapping")},
                       {"ss", QCoreApplication::translate(c, "Super sampling factor."),
                        QCoreApplication::translate(c, "factor")},
                       {"iter", QCoreApplication::translate(c, "Maximum number iterations."),
                        QCoreApplication::translate(c, "number")},
                       {"hyst", QCoreApplication::translate(c, "Algorithm hysteresis."),
                        QCoreApplication::translate(c, "value")},
                       {"hystd", QCoreApplication::translate(c, "Hysteresis delta per iteration."),
                        QCoreApplication::translate(c, "value")},
                       {"output", QCoreApplication::translate(c, "Output file."),
                        QCoreApplication::translate(c, "file")},
                       {"params", QCoreApplication::translate(c, "JSON parameter file."),
                        QCoreApplication::translate(c, "params")}});

    parser.process(app);

    /*
    auto ellipticalGauss2D = [](float x, float y,     //
                                float muX, float muY, //
                                float sigmaX, float sigmaY) -> float {
        return 1.0 / (2.0 * M_PI * sigmaX * sigmaY) *
               qExp(-(qPow(x - muX, 2.0) / (2.0 * sigmaX * sigmaX) +
                      qPow(y - muY, 2) / (2.0 * sigmaY * sigmaY)));
    };
    */

    auto ellipticalGauss2DAppox = [](float x, float y, //
                                     float sigmaX, float sigmaY) -> float {
        return qExp(-((x * x) / (2.0 * sigmaX * sigmaX) + (y * y) / (2.0 * sigmaY * sigmaY)));
    };

//    QImage gaussian(1920 * 3, 1080 * 3, QImage::Format_Grayscale8);
        QImage gaussian(1920, 1080 , QImage::Format_Grayscale8);
    for (int y = 0; y < gaussian.height(); ++y) {
        uchar * line = gaussian.scanLine(y);
        for (int x = 0; x < gaussian.width(); ++x) {
            float g = ellipticalGauss2DAppox(x - gaussian.width() / 2, y - gaussian.height() / 2, //
                                            179, 28);
            line[x] = qMin(static_cast<int>((1.0 - g) * 255.0), 254);
        }
    }

    QImage density = gaussian; // QImage(":/input/input1.jpg");
    LBGStippling::Params params;

    if (parser.isSet("input")) {
        density = QImage(parser.value("input"));
    }
    if (parser.isSet("ip")) {
        params.initialPoints = parser.value("ip").toInt();
    }
    if (parser.isSet("ips")) {
        params.initialPointSize = parser.value("ips").toFloat();
    }
    params.adaptivePointSize = !parser.isSet("cps");

    if (parser.isSet("psmin")) {
        params.pointSizeMin = parser.value("psmin").toFloat();
    }
    if (parser.isSet("psmax")) {
        params.pointSizeMax = parser.value("psmax").toFloat();
    }
    if (parser.isSet("psmapping")) {
        int index = parser.value("psmapping").toInt();
        using pmf = LBGStippling::PointMappingFunction;
        switch (index) {
        case 0:
            params.mapping = pmf::LINEAR;
            break;
        case 1:
            params.mapping = pmf::SQUAREROOT;
            break;
        case 2:
            params.mapping = pmf::EXPONENTIAL;
            break;
        case 3:
            params.mapping = pmf::SQUARE;
        }
    }
    if (parser.isSet("ss")) {
        params.superSamplingFactor = parser.value("ss").toInt();
    }
    if (parser.isSet("iter")) {
        params.maxIterations = parser.value("iter").toInt();
    }
    if (parser.isSet("hyst")) {
        params.hysteresis = parser.value("hyst").toFloat();
    }
    if (parser.isSet("hystd")) {
        params.hysteresisDelta = parser.value("hystd").toFloat();
    }
    if (parser.isSet("params")) {
        params.loadParametersJSON(parser.value("params"));
    }
    if (!parser.isSet("output")) {
        MainWindow* window = new MainWindow(density, params);
        window->show();
    } else {
        QString outputPath = parser.value("output");
        QStringList outputList = outputPath.split(",");
        QStringList inputList = parser.value("input").split(",");

        assert(inputList.size() > outputList.size());

        LBGStippling stippling = LBGStippling();

        if (outputList.size() == inputList.size()) {
            // for each input one output

            for (int i = 0; i < inputList.size(); ++i) {
                const QString& in = inputList.at(i);
                const QString& out = outputList.at(i);
                density = QImage(in);
                std::vector<Stipple> stipples = stippling.stipple(density, params);

                QSvgGenerator generator;
                generator.setFileName(out);
                generator.setSize(density.size());
                generator.setViewBox(QRectF(0, 0, density.width(), density.height()));
                generator.setTitle("Stippling Result");
                generator.setDescription("SVG File created by Weighted Linde-Buzo-Gray Stippling");

                QPainter painter;
                painter.begin(&generator);
                painter.setPen(Qt::NoPen);

                for (const auto& s : stipples) {
                    auto x = s.pos.x() * density.width();
                    auto y = s.pos.y() * density.height();
                    painter.setBrush(s.color);
                    painter.drawEllipse(QPointF(x, y), s.size / 2.0, s.size / 2.0);
                }

                painter.end();
            }
        } else if (outputList.size() == 1) {
            // merge all inputs into one output

            QSvgGenerator generator;
            generator.setFileName(outputList.at(0));
            generator.setSize(density.size());
            generator.setViewBox(QRectF(0, 0, density.width(), density.height()));
            generator.setTitle("Stippling Result");
            generator.setDescription("SVG File created by Weighted Linde-Buzo-Gray Stippling");

            QPainter painter;
            painter.begin(&generator);
            painter.setPen(Qt::NoPen);

            for (const QString& in : inputList) {
                density = QImage(in);
                std::vector<Stipple> stipples = stippling.stipple(density, params);

                for (const auto& s : stipples) {
                    auto x = s.pos.x() * density.width();
                    auto y = s.pos.y() * density.height();
                    painter.setBrush(s.color);
                    painter.drawEllipse(QPointF(x, y), s.size / 2.0, s.size / 2.0);
                }
            }
            painter.end();
        }
        exit(0);
    }

    return app.exec();
}
