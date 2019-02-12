#ifndef LBGSTIPPLING_H
#define LBGSTIPPLING_H

#include "voronoidiagram.h"

#include <random>

#include <QImage>
#include <QObject>
#include <QVector2D>
#include <QVector>

#include "voronoicell.h"

// TODO: Color is only used for debugging
struct Stipple {
    QVector2D pos;
    float size;
    QColor color;
};

class LBGStippling {

  public:
    enum PointMappingFunction { LINEAR = 0, SQUAREROOT = 1, EXPONENTIAL = 2, SQUARE = 3 };

    struct Params {
        int initialPoints = 10000;
        double initialPointSize = 1.0;

        bool adaptivePointSize = true;
        double pointSizeMin = 0.6;
        double pointSizeMax = 4.0;

        PointMappingFunction mapping = PointMappingFunction::SQUAREROOT;

        size_t superSamplingFactor = 1;
        size_t maxIterations = 42;

        double hysteresis = 0.5;
        double hysteresisDelta = 0.01;

        void saveParametersJSON(const QString& path);
        void loadParametersJSON(const QString& path);
    };

    struct Status {
        size_t iteration;
        size_t size;
        size_t splits;
        size_t merges;
        float hysteresis;
    };

    struct Result {
        std::vector<Stipple> stipples;
        IndexMap indexMap;
        std::vector<float> neighborWeightMap;
        std::vector<uint32_t > neighborIndexMap ;
        QList<unsigned int> point2morton;
    };

    template <class T>
    using Report = std::function<void(const T&)>;

    LBGStippling();

    Result stipple(const QImage& density, const Params& params, const int batchCount = 1,
                   const int batchNo = 0) const;

    // TODO: Rename and method chaining.
    void setStatusCallback(Report<Status> statusCB);
    void setStippleCallback(Report<std::vector<Stipple>> stippleCB);
    void setCellCallback(Report<IndexMap> cellCB);

  private:
    Report<Status> m_statusCallback;
    Report<std::vector<Stipple>> m_stippleCallback;
    Report<IndexMap> m_cellCallback;
};

#endif // LBGSTIPPLING_H
